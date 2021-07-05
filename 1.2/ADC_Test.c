#include <linux/module.h>
#include <linux/kernel.h>


MODULE_AUTHOR("grillow1337");
MODULE_DESCRIPTION("ADC_Test");
MODULE_VERSION("1.0.0");
MODULE_LICENSE("GPL");


int __init init_module()
{
    printk(KERN_INFO "ADC_Test started!\n");
    return 0;
}


void cleanup_module()
{
    printk(KERN_INFO "ADC_Test stopped!\n");
}
