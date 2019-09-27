

#include <linux/module.h>  // For module_init, module_exit
#include <linux/printk.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("DAVIT KALANTARYAN");

static int __init hello_world_test_init_module(void)
{
	pr_notice("Initing module!\n");
	return 0;
}

static void __exit hello_world_test_cleanup_module(void)
{
	pr_notice("Cleaning moule!\n");
}


module_init(hello_world_test_init_module);
module_exit(hello_world_test_cleanup_module);
