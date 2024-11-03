#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

//make a new device: sudo mknod /dev/mydevice c 90 0

//Test opening the device created
int main(){
    int dev = open("/dev/mydevice", O_RDONLY);
    if(dev == -1){
        printf("Opening was not possible! \n");
        return -1;
    }
    printf("Opening was sucessful! \n");
    close(dev);
    return 0;
}