#include "greenhouse_driver.h"


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Karina MG");
MODULE_DESCRIPTION("Driver for Greenhouse USB device");

/**
 * @brief Read data out of the buffer
 */
static ssize_t driver_read(struct file *File, char *user_buffer, size_t count, loff_t *offs) {
	int to_copy, not_copied, delta;

	/* Get amount of data to copy */
	to_copy = min(count, buffer_pointer);

	/* Copy data to user */
	not_copied = copy_to_user(user_buffer, buffer, to_copy);

	/* Calculate data */
	delta = to_copy - not_copied;

	return delta;
}

/**
 * @brief Write data to buffer
 */
static ssize_t driver_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs) {
	int to_copy, not_copied, delta;

	/* Get amount of data to copy */
	to_copy = min(count, sizeof(buffer));

	/* Copy data to user */
	not_copied = copy_from_user(buffer, user_buffer, to_copy);
	buffer_pointer = to_copy;

	/* Calculate data */
	delta = to_copy - not_copied;

	return delta;
}

/** 
 * @brief Function called when the device file is opened
 */
static int driver_open(struct inode *device_file, struct file *instance){
    printk("Greenhouse driver - open was called \n");
    return 0;
}

/** 
 * @brief Function called when the device file is closed
 */
static int driver_close(struct inode *device_file, struct file *instance){
    printk("Greenhouse driver - close was called \n");
    return 0;
}

static struct file *serial_file = NULL;

static int serial_open(void){
    serial_file = filp_open(SERIAL_DEVICE, O_RDWR | O_NOCTTY, 0);

    if(IS_ERR(serial_file)){
        printk(KERN_ALERT "Failed to open the serial device \n");
        return PTR_ERR(serial_file);
    }
    return 0;
}

static void serial_close(void){
    if(serial_file){
        filp_close(serial_file, NULL);
    }
}

static ssize_t serial_write(const char *data, size_t size){
    ssize_t ret;

    if(!serial_file){
        printk(KERN_ALERT "Serial device is not open \n");
        return -EIO;
    }

    ret = kernel_write(serial_file, data, size, &serial_file->f_pos);
    printk(KERN_ALERT "Greenhouse driver wrote: %zd bytes\n", ret);
    return ret;          
}

static ssize_t driver_ioctl(struct file *file, unsigned cmd, unsigned long arg){
	switch(cmd){
		case OPEN_WATER:
			serial_write("Abrir tubo", 10);
			break;
		case CLOSE_WATER:
			serial_write("Cerrar tubo", 11);
			break;
	}
	return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = driver_open,
    .release = driver_close,
    .read = driver_read,
    .write = driver_write,
	.unlocked_ioctl = driver_ioctl
};


static int __init greenhouse_driver_init(void){

    //Allocate device number
    if(alloc_chrdev_region(&device_num, 0, 1, DRIVER_NAME) < 0){
        printk("Device number could not be allocated! \n");
        return -1;
    }
    printk("Greenhouse - Device Number. Major: %d, Minor: %d was registered!\n", device_num >> 20, device_num & 0xfffff);

    // Create device class 
	if((greenhouse_class = class_create(THIS_MODULE,DRIVER_CLASS)) == NULL) {
		printk("Device class can not be created!\n");
		goto ClassError;
	}

	// create device file 
	if(device_create(greenhouse_class, NULL, device_num, NULL, DRIVER_NAME) == NULL) {
		printk("Can not create device file!\n");
		goto FileError;
	}

	// Initialize device file 
	cdev_init(&greenhouse_device, &fops);

	// Regisering device to kernel 
	if(cdev_add(&greenhouse_device, device_num, 1) == -1) {
		printk("Registering of device to kernel failed!\n");
		goto AddError;
	}
    
    
    if(serial_open()){
        printk(KERN_ALERT "Failed to open serial device! \n");
        goto AddError;
    }

    printk(KERN_INFO "Greenhouse serial driver initialized \n");
	return 0;

AddError:
	device_destroy(greenhouse_class, device_num);
FileError:
	class_destroy(greenhouse_class);
ClassError:
	unregister_chrdev_region(device_num, 1);
	return -1;
}

static void __exit greenhouse_driver_exit(void){
    serial_close();
    cdev_del(&greenhouse_device);
	device_destroy(greenhouse_class, device_num);
	class_destroy(greenhouse_class);
	unregister_chrdev_region(device_num, 1);
    printk(KERN_INFO "Greenhouse driver terminated \n");
}

module_init(greenhouse_driver_init);
module_exit(greenhouse_driver_exit);