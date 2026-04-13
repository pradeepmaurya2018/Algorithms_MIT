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
	{ 0x7db71bea, "free_netdev" },
	{ 0xb69d5d49, "ether_setup" },
	{ 0x747bd2f7, "consume_skb" },
	{ 0x7db71bea, "unregister_netdev" },
	{ 0xd272d446, "__fentry__" },
	{ 0xe8213e80, "_printk" },
	{ 0x058c185a, "jiffies" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0x82872ec5, "alloc_netdev_mqs" },
	{ 0x2f2f45a5, "register_netdev" },
	{ 0xbebe66ff, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0x7db71bea,
	0xb69d5d49,
	0x747bd2f7,
	0x7db71bea,
	0xd272d446,
	0xe8213e80,
	0x058c185a,
	0xd272d446,
	0x82872ec5,
	0x2f2f45a5,
	0xbebe66ff,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"free_netdev\0"
	"ether_setup\0"
	"consume_skb\0"
	"unregister_netdev\0"
	"__fentry__\0"
	"_printk\0"
	"jiffies\0"
	"__x86_return_thunk\0"
	"alloc_netdev_mqs\0"
	"register_netdev\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "012D842E57897F0709C9750");
