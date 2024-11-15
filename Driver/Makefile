#obj-m += usb_driver.o
#name = usb_driver

obj-m += arduino_driver.o
name = arduino_driver

#compile module
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

#run before reinstalling module
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

#load module into the kernel
load: 
	sudo insmod $(name).ko
	
#remove module from the kernel
remove: 
	sudo rmmod $(name)

check:
	lsmod | grep $(name)

log:
	sudo dmesg | tail

run:
	gcc c_test.c -o c_tes
	./test