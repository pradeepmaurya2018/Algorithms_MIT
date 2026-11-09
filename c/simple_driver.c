//
// Created by 2025 on 17-09-2026.
//


#include <linux/module.h>
#include <linux/init.h>

static int __init hello_init(void)
{
    pr_info("Hello, Linux driver!\n");
    return 0;
}

static void __exit hello_exit(void)
{
    pr_info("Goodbye, Linux driver!\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");