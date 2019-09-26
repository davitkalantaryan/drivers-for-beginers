

#include <linux/module.h>  // For module_init, module_exit
#include <linux/printk.h>

static int __init hello_world_test_init_module(void)
{
	//printk();
	pr_notice("hello world!\n");
	return 0;
}

static void __exit hello_world_test_cleanup_module(void)
{
}


module_init(hello_world_test_init_module);
module_exit(hello_world_test_cleanup_module);
