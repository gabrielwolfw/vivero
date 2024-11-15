#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "arduino_interface.h"

int main() {
	int answer;
	int dev = open("/dev/vivero", O_WRONLY);
	if(dev == -1) {
		printf("Opening was not possible!\n");
		return -1;
	}

	ioctl(dev, TURN_LED_ON, &answer);
    usleep(250000),
	ioctl(dev, TURN_LED_OFF, NULL);

	printf("Opening was successfull!\n");
	close(dev);
	return 0;
}