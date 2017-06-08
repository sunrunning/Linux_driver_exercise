#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <linux/fcntl.h>



int main()
{
	int fd;
	char buf[] = "This char driver demo!";
	
	char buf_read[1024];

	if (fd = open("./chardev.txt", O_RDWR | O_CREAT) == -1) {
		printf("Open chardev error!\n");
		goto failexit;
	} else {
		printf("Open chardev success!");
	}
	
	lseek(fd, 0, SEEK_SET);
	read(fd, buf_read, sizeof(buf));
	printf("Before buf is %s", buf_read);
	write(fd, buf, sizeof(buf));
	
	lseek(fd, 5, SEEK_SET);
	read(fd, buf_read, sizeof(buf));
	printf("After buf is %s", buf_read);

failexit:
	close(fd);
	return 0;
}
