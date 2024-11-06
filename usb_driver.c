#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb.h>

/* Meta information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Karina MG");
MODULE_DESCRIPTION("Driver for Arduino USB device");

#define VENDOR_ID 0x2341
#define PRODUCT_ID 0x0043
//idVendor           0x2341 Arduino SA
//idProduct          0x0043 Uno R3 (CDC ACM)
//lsusb -s 1:5 -v

static struct usb_device_id my_usb_table[] = {
    {USB_DEVICE(VENDOR_ID, PRODUCT_ID)},
    {}
};
MODULE_DEVICE_TABLE(usb, my_usb_table);

static int my_usb_probe(struct usb_interface *intf, const struct usb_device_id *id){
    printk("my_usb_devdrv - Probe finction \n");
    return 0;
}

static void my_usb_disconnect(struct usb_interface *intf){
    printk("my_usb_devdrv - Exit function \n");
}

static struct usb_driver my_usb_driver = {
    .name = "my_usb_devdrv",
    .id_table = my_usb_table,
    .probe = my_usb_probe,
    .disconnect = my_usb_disconnect,
};


/** 
 * @brief Function called when the module is loaded into the kernel
 */
static int __init ModuleInit(void){
    int result;
    printk("my_usb_devdrv - Init function \n");
    result = usb_register(&my_usb_driver);
    if(result){
        printk("my_usb_devdrv - Error during register \n");
        return -result;
    }
    return 0;
}

/** 
 * @brief Function called when the module is removed from the kernel
 */
static void __exit ModuleExit(void){
    printk("my_usb_devdrv - Disconnect function! \n");
    usb_deregister(&my_usb_driver);
}

module_init(ModuleInit);
module_exit(ModuleExit);