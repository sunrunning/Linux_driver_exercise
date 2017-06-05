#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

//This is License, must have this module 
MODULE_LICENSE("GPL");

//This is module load function
static int __init hello_init(void)
{
	printk(KERN_ALERT "Hello Kernel!\n");
	return 0;
}


//This is module unload function
static void __exit hello_exit(void)
{
	printk(KERN_ALERT "Goodbye Kernel!\n");
}

//Register the load function
module_init(hello_init);
module_exit(hello_exit);

//This is optional
MODULE_AUTHOR("John Lee");
MODULE_DESCRIPTION("This is a simple example");
MODULE_ALIAS("Simple example");
