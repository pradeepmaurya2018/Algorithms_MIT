//
// Created by pm on 4/16/26.
//
#include <linux/module.h>
#include <linux/init.h>

int my_init() {
    printk("hello world from  linix kernal\n");
    return 0;
}
int my_exit()
{
    printk("Kernal exit\n");
    return 0;
}

module_init(my_init);
module_exit(my_exit);
MODULE_LICENSE("GPL");