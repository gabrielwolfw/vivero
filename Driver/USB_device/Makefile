obj-m += usb-arduino.o
name = usb_arduino

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

#load module into the kernel
load: 
	sudo insmod usb-arduino.ko

#remove module from the kernel
remove: 
	sudo rmmod $(name)

cdc:
	sudo rmmod cdc_acm

check:
	lsmod | grep $(name)

log:
	sudo dmesg | tail

compile:
	gcc test.c -o test 

run:
	sudo ./test
