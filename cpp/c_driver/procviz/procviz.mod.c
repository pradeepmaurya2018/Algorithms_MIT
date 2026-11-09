#include <linux/module.h>
#include <linux/export-internal.h>
#include <linux/compiler.h>

MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xe931a49e, "single_open" },
	{ 0xbd03ed67, "__ref_stack_chk_guard" },
	{ 0xb61837ba, "seq_printf" },
	{ 0x90a48d82, "__ubsan_handle_out_of_bounds" },
	{ 0xd272d446, "__stack_chk_fail" },
	{ 0xd272d446, "__rcu_read_lock" },
	{ 0x0d428105, "init_task" },
	{ 0xd272d446, "__rcu_read_unlock" },
	{ 0x102ef4a9, "proc_remove" },
	{ 0xaa9a3b35, "seq_read" },
	{ 0x253f0c1d, "seq_lseek" },
	{ 0x34d5450c, "single_release" },
	{ 0xd272d446, "__fentry__" },
	{ 0x80222ceb, "proc_create" },
	{ 0xe8213e80, "_printk" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0xbebe66ff, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0xe931a49e,
	0xbd03ed67,
	0xb61837ba,
	0x90a48d82,
	0xd272d446,
	0xd272d446,
	0x0d428105,
	0xd272d446,
	0x102ef4a9,
	0xaa9a3b35,
	0x253f0c1d,
	0x34d5450c,
	0xd272d446,
	0x80222ceb,
	0xe8213e80,
	0xd272d446,
	0xbebe66ff,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"single_open\0"
	"__ref_stack_chk_guard\0"
	"seq_printf\0"
	"__ubsan_handle_out_of_bounds\0"
	"__stack_chk_fail\0"
	"__rcu_read_lock\0"
	"init_task\0"
	"__rcu_read_unlock\0"
	"proc_remove\0"
	"seq_read\0"
	"seq_lseek\0"
	"single_release\0"
	"__fentry__\0"
	"proc_create\0"
	"_printk\0"
	"__x86_return_thunk\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "137858EF586D1D2BA348211");
