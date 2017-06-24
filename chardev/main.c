#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <linux/fcntl.h>



int main()
{
	int fd;
	char buf[] = "This is a char demo~";
	
	char buf_read[1024];

	if ((fd = open("/dev/chardev", O_RDWR | O_CREAT)) == -1) {
		printf("Open chardev error!\n");
		goto failexit;
	} else {
		printf("Open chardev success!\n");
	 }
	
	lseek(fd, 0, SEEK_SET);
	read(fd, buf_read, sizeof(buf));
	printf("Before buf is:%s\n", buf_read);
	write(fd, buf, sizeof(buf));
	
	lseek(fd, 10, SEEK_SET);
	read(fd, buf_read, sizeof(buf));
	printf("After buf is:%s\n", buf_read);

failexit:
	close(fd);
	return 0;
}
