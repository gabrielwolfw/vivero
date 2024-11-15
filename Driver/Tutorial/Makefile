obj-m += main.o

#compile module
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

#run before reinstalling module
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

#load module into the kernel
load: 
	sudo insmod main.ko

#remove module from the kernel
remove: 
	sudo rmmod main

check:
	lsmod | grep main

log:
	sudo dmesg | tail

test:
	gcc test.c -o test
	./test
#cat /proc/devices | grep my_dev_nr
#ls /dev/ViveroDriver -al
#sudo chmod 666 /dev/ViveroDriver
#make a new device: sudo mknod /dev/mydevice c 90 0
#sudo rm /dev/mydevice
