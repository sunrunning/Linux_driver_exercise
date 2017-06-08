
#ifndef _CHARDEV_H_
#define _CHARDEV_H_


#ifndef CHARDEV_MAJOR
#define CHARDEV_MAJOR 260
#endif

#ifndef CHARDEV_NR_DEVS
#define CHARDEV_NR_DEVS 2
#endif


#ifndef CHARDEV_SIZE
#define CHARDEV_SIZE 4096
#endif


struct chardev
{
	char * data;
	unsigned long size;
};

#endif
