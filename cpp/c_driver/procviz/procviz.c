//
// Created by 2025 on 18-04-2026.
//
// procviz.c — Process tree + CFS scheduler inspector
// Concepts: task_struct, process tree traversal, container_of,
//           scheduler classes, vruntime, GFP flags, goto cleanup, /proc

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/sched/task.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/mm.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pradeep");
MODULE_DESCRIPTION("Process tree and scheduler inspector");

#define PROCVIZ_NAME "procviz"

static struct proc_dir_entry *procviz_entry;

/* Map task state to a human-readable string */
static const char *task_state_str(long state)
{
    if (state == TASK_RUNNING)
        return "R";
    if (state & TASK_UNINTERRUPTIBLE)
        return "D";
    if (state & TASK_INTERRUPTIBLE)
        return "S";
    if (state & __TASK_STOPPED)
        return "T";
    if (state & EXIT_ZOMBIE)
        return "Z";
    return "?";
}

/* Map scheduler policy to string */
static const char *sched_policy_str(unsigned int policy)
{
    switch (policy) {
    case SCHED_NORMAL:   return "CFS";
    case SCHED_FIFO:     return "FIFO";
    case SCHED_RR:       return "RR";
    case SCHED_BATCH:    return "BATCH";
    case SCHED_IDLE:     return "IDLE";
    case SCHED_DEADLINE: return "DL";
    default:             return "?";
    }
}

/*
 * Print one task's info. We use seq_file for /proc output — it handles
 * the buffering and pagination automatically.
 *
 * Note: we access task->se.vruntime directly. se = sched_entity,
 * the CFS-specific part of task_struct. vruntime is in nanoseconds.
 */
static void print_task(struct seq_file *m, struct task_struct *task, int depth)
{
    int i;
    char indent[64];
    int indent_len;

    /* Build indentation string */
    indent_len = min(depth * 2, 62);
    for (i = 0; i < indent_len; i++)
        indent[i] = ' ';
    indent[indent_len] = '\0';

    seq_printf(m,
        "%s[%6d] %-20s  st=%-2s  pol=%-5s  vrt=%llu  prio=%3d  nr_thr=%d\n",
        indent,
        task->pid,
        task->comm,
        task_state_str(task->__state),
        sched_policy_str(task->policy),
        task->se.vruntime,
        task->prio,
        get_nr_threads(task));
}

/*
 * Recursively walk the process tree from a given task.
 * Uses task->children (list_head) and task->sibling.
 * container_of is used internally by list_for_each_entry.
 *
 * We hold RCU read lock — required any time we traverse task lists.
 */
static void walk_tree(struct seq_file *m, struct task_struct *task, int depth)
{
    struct task_struct *child;

    print_task(m, task, depth);

    /*
     * list_for_each_entry(child, &task->children, sibling)
     * expands container_of internally:
     *   child = container_of(pos, struct task_struct, sibling)
     * This recovers the task_struct from the embedded sibling list_head.
     */
    list_for_each_entry(child, &task->children, sibling) {
        walk_tree(m, child, depth + 1);
    }
}

/*
 * /proc read handler — called when userspace reads /proc/procviz
 */
static int procviz_show(struct seq_file *m, void *v)
{
    seq_printf(m, "%-8s %-20s  %-4s %-6s %-20s %-5s %-6s\n",
               "PID", "NAME", "ST", "POL", "VRUNTIME(ns)", "PRIO", "NTHRD");
    seq_printf(m, "%s\n",
               "--------------------------------------------------------------------");

    /*
     * RCU read lock protects task list traversal.
     * We must not sleep inside rcu_read_lock().
     * This is context-sensitive allocation: if we needed memory here,
     * we'd use GFP_ATOMIC, not GFP_KERNEL.
     */
    rcu_read_lock();
    walk_tree(m, &init_task, 0);
    rcu_read_unlock();

    return 0;
}

static int procviz_open(struct inode *inode, struct file *file)
{
    return single_open(file, procviz_show, NULL);
}

static const struct proc_ops procviz_fops = {
    .proc_open    = procviz_open,
    .proc_read    = seq_read,
    .proc_lseek   = seq_lseek,
    .proc_release = single_release,
};

static int __init procviz_init(void)
{
    /*
     * goto cleanup pattern (Ch 2):
     * If proc_create fails, we jump to the appropriate cleanup label.
     * Here it's simple, but the pattern scales to multi-step init.
     */
    procviz_entry = proc_create(PROCVIZ_NAME, 0444, NULL, &procviz_fops);
    if (!procviz_entry) {
        pr_err("procviz: failed to create /proc/%s\n", PROCVIZ_NAME);
        goto out_err;
    }

    pr_info("procviz: loaded. Read /proc/%s\n", PROCVIZ_NAME);
    return 0;

out_err:
    return -ENOMEM;
}

static void __exit procviz_exit(void)
{
    proc_remove(procviz_entry);
    pr_info("procviz: unloaded\n");
}

module_init(procviz_init);
module_exit(procviz_exit);