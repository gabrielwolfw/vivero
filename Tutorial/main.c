#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h> //To access the file system
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
/* Meta information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Karina MG");
MODULE_DESCRIPTION("Hello world module");

/* Buffer for data */
static char buffer[255];
static size_t buffer_pointer;

/* Variables for device and device class */
static dev_t my_device_num;
static struct class *my_class;
static struct cdev my_device;

#define DRIVER_NAME "dummydriver"
#define DRIVER_CLASS "MyModuleClass"


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
    printk("dev_nr - open was called \n");
    return 0;
}

/** 
 * @brief Function called when the device file is closed
 */
static int driver_close(struct inode *device_file, struct file *instance){
    printk("dev_nr - close was called \n");
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = driver_open,
    .release = driver_close,
    .read = driver_read,
    .write = driver_write
};

#define MYMAJOR 90
/** 
 * @brief Function called when the module is loaded into the kernel
 */
static int __init ModuleInit(void){
    int retval;
    printk("Hello, Kernel! \n");
    /* Register a device number*/

    //Allocate device number
    if(alloc_chrdev_region(&my_device_num, 0, 1, DRIVER_NAME) < 0){
        printk("Device number could not be allocated! \n");
        return -1;
    }
    printk("read_write - Device Nr. Major: %d, Minor: %d was registered!\n", my_device_num >> 20, my_device_num & 0xfffff);

    /* Create device class */
	if((my_class = class_create(DRIVER_CLASS)) == NULL) {
		printk("Device class can not be created!\n");
		goto ClassError;
	}

	/* create device file */
	if(device_create(my_class, NULL, my_device_num, NULL, DRIVER_NAME) == NULL) {
		printk("Can not create device file!\n");
		goto FileError;
	}

	/* Initialize device file */
	cdev_init(&my_device, &fops);

	/* Regisering device to kernel */
	if(cdev_add(&my_device, my_device_num, 1) == -1) {
		printk("Registering of device to kernel failed!\n");
		goto AddError;
	}

	//GPIO 8 init
	if(gpio_request(8, "rpi-gpio-4")){
		printk("Can not allocate GPIO 4 \n");
		goto AddError;
	}

	//set GPIO direction
	if(gpio_direction_output(4,0)){
		printk("Can not set GPIO 4 to output!")
	}

	return 0;

AddError:
	device_destroy(my_class, my_device_num);
FileError:
	class_destroy(my_class);
ClassError:
	unregister_chrdev_region(my_device_num, 1);
	return -1;
}

/** 
 * @brief Function called when the module is removed from the kernel
 */
static void __exit ModuleExit(void){
    cdev_del(&my_device);
	device_destroy(my_class, my_device_num);
	class_destroy(my_class);
	unregister_chrdev_region(my_device_num, 1);
    printk("Goodbye, Kernel! \n");
}

module_init(ModuleInit);
module_exit(ModuleExit);