#ifndef GREENHOUSE_DRIVER_H
#define GREENHOUSE_DRIVER_H

#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/tty.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/ioctl.h>
#include "greenhouse_interface.h"

/* Meta information */


static int serial_open(void);
static void serial_close(void);
static ssize_t serial_write(const char *data, size_t size);

static ssize_t driver_read(struct file *File, char *user_buffer, size_t count, loff_t *offs);
static ssize_t driver_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs);
static int driver_open(struct inode *device_file, struct file *instance);
static int driver_close(struct inode *device_file, struct file *instance);

static int __init greenhouse_driver_init(void);
static void __exit greenhouse_driver_exit(void);

static long driver_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

#define SERIAL_DEVICE "/dev/ttyACM0"

// Buffer for data 
static char buffer[255];
static size_t buffer_pointer;

// Variables for device and device class 
static dev_t device_num;
static struct class *greenhouse_class;
static struct cdev greenhouse_device;

#define DRIVER_NAME "GreenhouseDriver"
#define DRIVER_CLASS "GreenhouseClass"

#endif