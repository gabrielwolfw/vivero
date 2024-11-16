#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <libusb-1.0/libusb.h>

#define VENDOR_ID 0x2341
#define PRODUCT_ID 0x0043

int main(int argc, char **argv){
  int status, value;
  libusb_device_handle *dev = NULL;
  char buffer[1];
  int transferred;

  status = libusb_init(NULL);
  if(status != 0){
    perror("Error init libusb");
    return -status;
  }
  //libusb_get_device_list();
  dev = libusb_open_device_with_vid_pid(NULL, VENDOR_ID, PRODUCT_ID);
  if(dev == NULL){
    printf("Error! Could not find USB device \n");  
    libusb_exit(NULL);
    return -1;
  }

  buffer[0] = 0;
  for(int i=0; i <5; i++){
    buffer[0] ^= 0x1; 
    status = libusb_bulk_transfer(dev, 0x04, buffer, 1, &transferred, 100);
    printf("Status: %d, Bytes transferrred %d \n", status, transferred);
    usleep(250000);
  }

  libusb_close(dev);
  libusb_exit(NULL);
  return 0;
}