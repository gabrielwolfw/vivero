#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
//This interface contains the commands available for ioctl
#include "greenhouse_interface.h"

int main() {
	
	//Must create the file using the Major number registered by the driver
	//with command sudo mknod /dev/vivero c 234 0
	int dev = open("/dev/ard0", O_WRONLY);
	if(dev == -1) {
		printf("Opening was not possible!\n");
		return -1;
	}

	printf("Now blinking LED!\n");
	int val = 1;
	ioctl(dev, OPEN_WATER, &val);
    usleep(500000);
	val = 0;
	ioctl(dev, CLOSE_WATER, &val);

	close(dev);
	return 0;
}