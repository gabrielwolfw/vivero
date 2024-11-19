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
	//fprintf(dev, "1");
	//IOCTL sends command to device with device number Major registered by the the driver
	int val = 2;
	ioctl(dev, TURN_LED_ON, &val);
    usleep(250000);
	//int val2 = 0;
	//ioctl(dev, TURN_LED_OFF, &val2);
	//fprintf(dev, "0");

	close(dev);
	return 0;
}