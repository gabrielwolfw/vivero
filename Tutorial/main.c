#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h> //To access the file system

/* Meta information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Karina MG");
MODULE_DESCRIPTION("Hello world module");


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
    .release = driver_close
};

#define MYMAJOR 90
/** 
 * @brief Function called when the module is loaded into the kernel
 */
static int __init ModuleInit(void){
    int retval;
    printk("Hello, Kernel! \n");
    /* Register a device number*/
    retval = register_chrdev(MYMAJOR, "my_dev_num", &fops);
    if(retval == 0){
        printk("device num - registered Device number Major: %d, Minor: %d \n", MYMAJOR, 0);
    }
    else if(retval > 0){
        printk("device num - registered Device number Major: %d, Minor: %d \n", retval >> 20, retval&0xfffff);
    }
    else{
        printk("Could not register device number \n");
        return -1;
    }
    return 0;
}

/** 
 * @brief Function called when the module is removed from the kernel
 */
static void __exit ModuleExit(void){
    unregister_chrdev(MYMAJOR, "my_dev_num");
    printk("Goodbye, Kernel! \n");
}

module_init(ModuleInit);
module_exit(ModuleExit);