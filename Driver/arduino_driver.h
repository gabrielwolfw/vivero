#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/tty.h>
#include <linux/slab.h>
#include <linux/ioctl.h>

/* Meta information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Karina MG");
MODULE_DESCRIPTION("Driver for Arduino USB device");

static int serial_open(void);
static void serial_close(void);
static ssize_t serial_write(const char *data, size_t size);

static int __init arduino_driver_init(void);
static void __exit arduino_driver_exit(void);

static ssize_t turn_on_led(const char *buffer, size_t len);

static long my_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

#define SERIAL_DEVICE "/dev/ttyACM0"
#define MY_IOCTL_CMD _IOW('a', 'a', int32_t*)

static const struct file_operations fops = {
    .unlocked_ioctl = my_ioctl,
    .owner = THIS_MODULE
};