obj-m += greenhouse_driver.o
name = greenhouse_driver

#compile module
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

#run before reinstalling module
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

#load module into the kernel
load: 
	sudo insmod $(name).ko
	
device:
	sudo mknod /dev/vivero c $(MAJOR) 0
	sudo chmod 666 /dev/vivero

#remove module from the kernel
remove: 
	sudo rmmod $(name)
	sudo rm /dev/vivero

#check if module was loaded into the kernel
check:
	lsmod | grep $(name)

#check kernel log, driver messages are printer there
log:
	sudo dmesg | tail

#compile interface test
comp:
	gcc interface_test.c -o test
	
run:
	./test

