#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/uaccess.h>
#include <linux/types.h>
#include <asm/io.h>

#define MAJOR_NUM 0
#define DEVICE_NAME "rpigpio"
#define CLASS_NAME DEVICE_NAME

#define PERIPHERAL_BASE 0x3F000000UL
#define GPIO_BASE (PERIPHERAL_BASE + 0x200000)

void* gpio_ctr = NULL;

void set_gpio_pullup(void *gpio_ctr, int gpio_nr) 
{
	int reg_id = gpio_nr / 32;
	int pos = gpio_nr % 32;
	#define GPIO_PUD_OFFSET 0x94
	#define GPIO_PUDCLK_OFFSET 0x98
	unsigned int* pud_reg = (unsigned int*) (gpio_ctr + GPIO_PUD_OFFSET);
	unsigned int* pudclk_reg = (unsigned int*) (gpio_ctr + GPIO_PUDCLK_OFFSET + 0x4 * reg_id);
	#define GPIO_PUD_PULLUP 0x2
	*pud_reg = GPIO_PUD_PULLUP;
	udelay(1);
	*pudclk_reg = (0x1 << pos);
	udelay(1);
	*pud_reg = 0;
	*pudclk_reg = 0;
}

static int rpigpio_open(struct inode *inode, struct file *f)
{
        pr_info("rpigpio_open\n");
        return 0;
}

static int rpigpio_release(struct inode *inode, struct file *f)
{
        pr_info("rpigpio_release\n");
        return 0;
}

char gpiobuf[1000];

ssize_t rpigpio_read(struct file *f, char __user *buf, size_t size, loff_t * off)
{
        unsigned long res;
        size_t sz = strlen(gpiobuf);
        res = copy_to_user(buf, gpiobuf, sz);
        strcpy(gpiobuf, "");
        pr_info("rpigpio_read\n");
        return sz;
}

ssize_t rpigpio_write(struct file *f, const char __user *buf, size_t size, loff_t * off)
{
        char my_buf[1000];
        unsigned long res;
        size_t i;
        res = copy_from_user(my_buf, buf, size);
        for(i = 0 ; i<size ; i++){
                if(my_buf[i] == 'r'){
                        gpio_set_value(13,0);
                        gpio_set_value(19,0);
                        gpio_set_value(26,1);
                        msleep(1000);
                }
                else if(my_buf[i] == 'g'){
                        gpio_set_value(13,0);
                        gpio_set_value(19,1);
                        gpio_set_value(26,0);
                        msleep(1000);
                }
                else if(my_buf[i] == 'b'){
                        gpio_set_value(13,1);
                        gpio_set_value(19,0);
                        gpio_set_value(26,0);
                        msleep(1000);
                }
                else if(my_buf[i] == 'o'){
                        gpio_set_value(13,0);
                        gpio_set_value(19,0);
                        gpio_set_value(26,0);
                        msleep(1000);
                }
        }
        return size;
}

static long rpigpio_ioctl(struct file *f, uint32_t cmd, unsigned long arg){
        switch(cmd){
                case 100:
                        {
                                uint32_t value[2];
                                int res;
                                value[0] = gpio_get_value(20);
                                value[1] = gpio_get_value(21);                                
                                res =copy_to_user((void*)arg, (void*) value, sizeof(uint32_t)*2);
                                break;
                        }
                case 101:
                        {
                                uint32_t value[3];
                                int res;
                                res = copy_from_user((void*) value, (void*) arg, sizeof(uint32_t)*3);

                                gpio_set_value(13,(int)value[0]);
                                gpio_set_value(19,(int)value[1]);
                                gpio_set_value(26,(int)value[2]);
                                break;
                        }
                default:
                        return -1;
        }
        return 0;
}


struct file_operations rpigpio_fops = {
        .open = rpigpio_open,
        .release = rpigpio_release,
        .unlocked_ioctl = rpigpio_ioctl,
        .read = rpigpio_read,
        .write = rpigpio_write,
};

static irqreturn_t gpio20_irq(int irq, void *dev_id)
{
        strcat(gpiobuf, "1");                     
        return IRQ_HANDLED;
}

static irqreturn_t gpio21_irq(int irq, void *dev_id)
{
        strcat(gpiobuf, "0");
        return IRQ_HANDLED;
}

static struct class *rpigpio_class = NULL;
static struct device *rpigpio_device = NULL;
static int majorNum;
void* gpio_ctr;
int ret;
static int __init rpigpio_init(void){
        majorNum = register_chrdev(MAJOR_NUM, DEVICE_NAME, &rpigpio_fops);
        if(majorNum < MAJOR_NUM){
                pr_alert("rpigpio Failed to register a major number\n");
                return majorNum;
        }

        rpigpio_class = class_create(THIS_MODULE, CLASS_NAME);
        rpigpio_device = device_create(rpigpio_class, NULL, MKDEV(majorNum,0), NULL, DEVICE_NAME);
       
        gpio_request(13, "gpio13");
        gpio_request(19, "gpio19");
        gpio_request(26, "gpio26");
        gpio_request(20, "gpio20");
        gpio_request(21, "gpio21");

        gpio_direction_output(13,0);
        gpio_direction_output(19,0);
        gpio_direction_output(26,0);        

        gpio_direction_input(20);
        gpio_direction_input(21);
        
        gpio_ctr = ioremap(GPIO_BASE,0x1000);

        set_gpio_pullup(gpio_ctr,20);
        set_gpio_pullup(gpio_ctr,21);
        
        strcpy(gpiobuf, "");

        ret = request_irq(gpio_to_irq(20), gpio20_irq, IRQF_TRIGGER_FALLING, "rpigpio20", NULL);
        if(ret < 0){
                pr_alert("%s: request_irq failed with %d\n", __func__, ret);
        }
        ret = request_irq(gpio_to_irq(21), gpio21_irq, IRQF_TRIGGER_FALLING, "rpigpio21", NULL);
        if(ret < 0){
                pr_alert("%s: request_irq failed with %d\n", __func__, ret);
        }

        pr_info("rpigpio module installed\n");
        return 0;
}

static void __exit rpigpio_exit(void){
        free_irq(gpio_to_irq(20), NULL);
        free_irq(gpio_to_irq(21), NULL);

        gpio_free(13);
        gpio_free(19);
        gpio_free(26);
        gpio_free(20);
        gpio_free(21);

        iounmap(gpio_ctr);

        device_destroy(rpigpio_class, MKDEV(majorNum, 0));
        class_unregister(rpigpio_class);
        class_destroy(rpigpio_class);
        unregister_chrdev(majorNum, DEVICE_NAME);
}

module_init(rpigpio_init);
module_exit(rpigpio_exit);
MODULE_LICENSE("GPL");
