#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <stdbool.h>


MODULE_AUTHOR("grillow1337");
MODULE_DESCRIPTION("ADC_Test");
MODULE_VERSION("1.0.0");
MODULE_LICENSE("GPL");

const char *DEVICE_NAME = "ADC_Test";

int Major;
bool is_open = false;

int32_t number = 0;


static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);

static struct file_operations fops = {
    .open = device_open,
    .read = device_read,
    .release = device_release
};


int __init init_module()
{
    printk(KERN_INFO "ADC_Test loaded!\n");
    Major = register_chrdev(0, DEVICE_NAME, &fops);
    if (Major < 0) {
        printk(KERN_ALERT "Registering char device %s failed with %d\n",
                DEVICE_NAME, Major);
        return Major;
    }

    printk(KERN_INFO "Major: %d", Major);
    
    return 0;
}


void cleanup_module()
{
    unregister_chrdev(Major, DEVICE_NAME);
    printk(KERN_INFO "ADC_Test unloaded!\n");
}


int device_open(struct inode *inode, struct file *file)
{
    if (is_open) {
        return -EBUSY;
    }
    is_open = true;
    return 0;
}


int device_release(struct inode *inode, struct file *file)
{
    is_open = false;
    return 0;
}


ssize_t device_read(struct file *filp, char *buffer,
        size_t length, loff_t *offset)
{
    size_t read = sprintf(buffer, "%d\n", number++);

    return read;
}
