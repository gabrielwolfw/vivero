#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/tty.h>
#include <linux/slab.h>

/* Meta information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Karina MG");
MODULE_DESCRIPTION("Driver for Arduino USB device");

#define SERIAL_DEVICE "/dev/ttyACM0"


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
        printk(KERN_ALERT "Serial device not opened \n");
        return -EIO;
    }

    ret = vfs_write(serial_file, data, size, &serial_file->f_pos);
    return ret;          
}

static ssize_t blink_led(const char *buffer, size_t len){
    char command = buffer[0];

    if(command == '1'){
        serial_write("1", 1);
    }
    else if(command == "0"){
        serial_write("0", 1);
    }

    return len;
}

static int __init arduino_driver_init(void){
    printk(KERN_INFO "Arduino serial driver initialized \n");

    if(serial_open()){
        printk(KERN_ALERT "Failed to open serial device \n");
        return -1;
    }

    return 0;
}

static void __exit arduino_driver_exit(void){
    serial_close();
    printk(KERN_INFO "Arduino driver terminated \n");
}

module_init(arduino_driver_init);
module_exit(arduino_driver_exit);