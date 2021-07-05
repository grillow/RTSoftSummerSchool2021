#include <linux/module.h>
#include <linux/kernel.h>


MODULE_LICENSE("GPL");

int init_module() {
    printk(KERN_INFO "ADC_Test started!\n");
    return 0;
}


void cleanup_module() {
    printk(KERN_INFO "ADC_Test stopped!\n");
}
