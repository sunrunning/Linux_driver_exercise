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
//#include <asm/system.h>
#include <asm/uaccess.h>
#include <linux/slab.h>

#include "chardev.h"

static int chardev_major = CHARDEV_MAJOR;
module_param(chardev_major, int, S_IRUGO);

struct chardev *char_devp;

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
	dev = &char_devp[num];
	
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

		printk(KERN_INFO "read %d bytes from %ld\n", count, p);
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

		printk(KERN_INFO "written %d bytes from %ld\n", count, p);
	}
	return ret;
}

//Seek file location position
static loff_t chardev_llseek(struct file *filp, loff_t offset, int whence)
{
	loff_t newpos;

	switch(whence) {
		case 0://Seek set
			newpos = offset;
			break;
		case 1://Seek current
			newpos = filp->f_pos + offset;
			break;
		case 2://Seek end
			newpos = CHARDEV_SIZE - 1 + offset;
			break;
		default://not happen
			return -EINVAL;
	}
	if ((newpos < 0) || (newpos > CHARDEV_SIZE)) {
		return -EINVAL;
	}

	filp->f_pos = newpos;
	return newpos;
}

//file operation struct
static const struct file_operations chardev_fops = {
	.owner = THIS_MODULE,
	.llseek = chardev_llseek,
	.read = chardev_read,
	.write = chardev_write,
	.open = chardev_open,
	.release = chardev_release,
};

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
	
	cdev_init(&cdev, &chardev_fops);
	cdev.owner = THIS_MODULE;
	cdev.ops = &chardev_fops;
	
	cdev_add(&cdev, MKDEV(chardev_major, 0), CHARDEV_NR_DEVS);
	
	//Apply kernel memory for char dev struct
	char_devp = kmalloc(CHARDEV_NR_DEVS * sizeof(struct chardev), GFP_KERNEL);
	if (!char_devp) {
		result = -ENOMEM;
		goto fail_malloc;
	}
	memset(char_devp, 0, sizeof(struct chardev));

	for (i = 0; i < CHARDEV_NR_DEVS; i++) {
		char_devp[i].size = CHARDEV_SIZE;
		//Real data memory space is applied here
		char_devp[i].data = kmalloc(CHARDEV_SIZE, GFP_KERNEL);
		memset(char_devp[i].data, 0, CHARDEV_SIZE);
	}
	return 0;
fail_malloc:
	unregister_chrdev_region(devno, 1);
	return result;
}


//This is module unload function
static void __exit chardev_exit(void)
{
	cdev_del(&cdev);
	kfree(char_devp);//Free char dev struct memory
	unregister_chrdev_region(MKDEV(chardev_major, 0), 2);//Release device number
}

//Register the load function
module_init(chardev_init);
module_exit(chardev_exit);

//This is optional
MODULE_AUTHOR("John Lee");
MODULE_DESCRIPTION("This is a simple example");
MODULE_ALIAS("Simple example");
