#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/cdev.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/uaccess.h>

#include "chardev.h"

static chardev_mahor = MEMDEV_MAJOR;

struct chardev *chardevp;

struct cdev cdev;

//This is License, must have this module 
MODULE_LICENSE("GPL");

//Open file function
int chardev_open(struct inode *inode, struct file *filp)
{
	struct chardev *dev;
	
	//Get associate device number
	int num = MINOR(inode->i_rdev);
	
	if (num > CHARDEV_NR_DEVS) {
		return -ENODEV;
	}
	dev = &chardevp[num];
	
	//
	filp->private_data = dev;

	return 0;
}

//Release File function
int chardev_release(struct inode *inode, struct file *filp)
{
	return 0;
}

//Read function
static ssize_t chardev_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos)
{
	unsigned long p = *ppos;
	unsigned int count = size;
	int ret = 0;
	struct chardev *dev = filp->private_data;//Get device struct point
	
	//Check the position is valied
	if (p >= CHARDEV_SIZE) {
		return 0;
	}
	if (count > CHARDEV_SIZE - p) {
		count = CHARDEV_SIZE - p;
	}

	//Get data from kernel to user space
	if (copy_to_user(buf, (void *)(dev->data + p), count)) {
		return -EFAULT;
	} else {
		*ppos += count;
		ret = count;

		printk(KERN_INFO "read %d bytes from %d\n", count, p);
	}
	return ret;
}

//Write function
static ssize_t chardev_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
	unsigned long p = *ppos;
	unsigned int count = size;
	int ret = 0;
	struct chardev *dev = filp->private_data;//Get device struct point
	
	//Check the position is valied
	if (p >= CHARDEV_SIZE) {
		return 0;
	}
	if (count > CHARDEV_SIZE - p) {
		count = CHARDEV_SIZE - p;
	}
	
	//Write data from user to kernel space
	if (copy_from_user(dev->data + p, buf, count)) {
		return -EFAULT;
	} else {
		*ppos += count;
		ret = count;

		printk(KERN_INFO "written %d bytes from %d\n", count, p);
	}
	return ret;
}

//This is module load function
static int __init chardev_init(void)
{
	int result, i;

	dev_t devno =MKDEV(chardev_major, 0);

	if (chardev_major) {
		result = register_chrdev_region(devno, 2, "chardev");
	} else {
		result = alloc_chrdev_region(&devno, 0, 2, "chardev");
		chardev_major = MAJOR(devno);
	}

	if (result < 0) {
		return result;
	}

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
