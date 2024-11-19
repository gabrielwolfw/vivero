#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
//THis interface contains the commands available for ioctl
#include "greenhouse_interface.h"

int main() {
	
	//Must create the file using the Major number registered by the driver
	//with command sudo mknod /dev/vivero c 234 0
	int dev = open("/dev/vivero", O_WRONLY);
	if(dev == -1) {
		printf("Opening was not possible!\n");
		return -1;
	}

	printf("Now blinking LED!\n");
	//IOCTL sends command to device with device number Major registered by the the driver
	ioctl(dev, OPEN_WATER, NULL);
    usleep(250000),
	ioctl(dev, CLOSE_WATER, NULL);


	close(dev);
	return 0;
}