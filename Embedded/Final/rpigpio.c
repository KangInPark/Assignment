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
#include <linux/string.h>
#include <asm/io.h>
#include <linux/jiffies.h>

#define MAJOR_NUM 0
#define DEVICE_NAME "rpigpio"
#define CLASS_NAME DEVICE_NAME

#define PERIPHERAL_BASE 0x3F000000UL
#define GPIO_BASE (PERIPHERAL_BASE + 0x200000)
#define JIF_VAL 30

void *gpio_ctr = NULL;

void set_gpio_pullup(void *gpio_ctr, int gpio_nr)
{
        int reg_id = gpio_nr / 32;
        int pos = gpio_nr % 32;
#define GPIO_PUD_OFFSET 0x94
#define GPIO_PUDCLK_OFFSET 0x98
        unsigned int *pud_reg = (unsigned int *)(gpio_ctr + GPIO_PUD_OFFSET);
        unsigned int *pudclk_reg = (unsigned int *)(gpio_ctr + GPIO_PUDCLK_OFFSET + 0x4 * reg_id);
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

char gpiobuf[1000000];

ssize_t rpigpio_read(struct file *f, char __user *buf, size_t size, loff_t *off)
{
        unsigned long res;
        char *output = gpiobuf + *off;
        ssize_t sz = strlen(output);
        if (size < sz)
        {
                res = copy_to_user(buf, output, size);
                *off += size;
        }
        else
        {
                res = copy_to_user(buf, output, sz);
                *off += sz;
        }
        pr_info("rpigpio_read\n");
        return sz;
}

ssize_t rpigpio_write(struct file *f, const char __user *buf, size_t size, loff_t *off)
{
        char my_buf[1000];
        unsigned long res;
        res = copy_from_user(my_buf, buf, size);
        if (my_buf[0] == 'r')
        {
                strcpy(gpiobuf, "");
        }
        return size;
}

struct file_operations rpigpio_fops = {
    .open = rpigpio_open,
    .release = rpigpio_release,
    .read = rpigpio_read,
    .write = rpigpio_write,
};

unsigned long irq5_time = 0, irq6_time = 0, irq13_time = 0, irq19_time = 0, irq26_time = 0, irq16_time = 0, irq20_time = 0, irq21_time = 0;
static irqreturn_t gpio5_irq(int irq, void *dev_id)
{
        if (get_jiffies_64() > irq5_time)
        {
                irq5_time = get_jiffies_64() + JIF_VAL;
        }
        else
        {
                return IRQ_HANDLED;
        }
        if (strlen(gpiobuf) == 1000000)
        {
                strcpy(gpiobuf, "");
        }
        strcat(gpiobuf, "0");
        return IRQ_HANDLED;
}

static irqreturn_t gpio6_irq(int irq, void *dev_id)
{
        if (get_jiffies_64() > irq6_time)
        {
                irq6_time = get_jiffies_64() + JIF_VAL;
        }
        else
        {
                return IRQ_HANDLED;
        }
        if (strlen(gpiobuf) == 1000000)
        {
                strcpy(gpiobuf, "");
        }
        strcat(gpiobuf, "1");
        return IRQ_HANDLED;
}
static irqreturn_t gpio13_irq(int irq, void *dev_id)
{
        if (get_jiffies_64() > irq13_time)
        {
                irq13_time = get_jiffies_64() + JIF_VAL;
        }
        else
        {
                return IRQ_HANDLED;
        }
        if (strlen(gpiobuf) == 1000000)
        {
                strcpy(gpiobuf, "");
        }
        strcat(gpiobuf, "2");
        return IRQ_HANDLED;
}

static irqreturn_t gpio19_irq(int irq, void *dev_id)
{
        if (get_jiffies_64() > irq19_time)
        {
                irq19_time = get_jiffies_64() + JIF_VAL;
        }
        else
        {
                return IRQ_HANDLED;
        }
        if (strlen(gpiobuf) == 1000000)
        {
                strcpy(gpiobuf, "");
        }
        strcat(gpiobuf, "3");
        return IRQ_HANDLED;
}
static irqreturn_t gpio26_irq(int irq, void *dev_id)
{
        if (get_jiffies_64() > irq26_time)
        {
                irq26_time = get_jiffies_64() + JIF_VAL;
        }
        else
        {
                return IRQ_HANDLED;
        }
        if (strlen(gpiobuf) == 1000000)
        {
                strcpy(gpiobuf, "");
        }
        strcat(gpiobuf, "4");
        return IRQ_HANDLED;
}

static irqreturn_t gpio16_irq(int irq, void *dev_id)
{
        if (get_jiffies_64() > irq16_time)
        {
                irq16_time = get_jiffies_64() + JIF_VAL;
        }
        else
        {
                return IRQ_HANDLED;
        }
        if (strlen(gpiobuf) == 1000000)
        {
                strcpy(gpiobuf, "");
        }
        strcat(gpiobuf, "5");
        return IRQ_HANDLED;
}
static irqreturn_t gpio20_irq(int irq, void *dev_id)
{
        if (get_jiffies_64() > irq20_time)
        {
                irq20_time = get_jiffies_64() + JIF_VAL;
        }
        else
        {
                return IRQ_HANDLED;
        }
        if (strlen(gpiobuf) == 1000000)
        {
                strcpy(gpiobuf, "");
        }
        strcat(gpiobuf, "6");
        return IRQ_HANDLED;
}

static irqreturn_t gpio21_irq(int irq, void *dev_id)
{
        if (get_jiffies_64() > irq21_time)
        {
                irq21_time = get_jiffies_64() + JIF_VAL;
        }
        else
        {
                return IRQ_HANDLED;
        }
        if (strlen(gpiobuf) == 1000000)
        {
                strcpy(gpiobuf, "");
        }
        strcat(gpiobuf, "7");
        return IRQ_HANDLED;
}
static struct class *rpigpio_class = NULL;
static struct device *rpigpio_device = NULL;
static int majorNum;
void *gpio_ctr;
int ret;
static int __init rpigpio_init(void)
{
        majorNum = register_chrdev(MAJOR_NUM, DEVICE_NAME, &rpigpio_fops);
        if (majorNum < MAJOR_NUM)
        {
                pr_alert("rpigpio Failed to register a major number\n");
                return majorNum;
        }

        rpigpio_class = class_create(THIS_MODULE, CLASS_NAME);
        rpigpio_device = device_create(rpigpio_class, NULL, MKDEV(majorNum, 0), NULL, DEVICE_NAME);

        gpio_request(5, "gpio5");
        gpio_request(6, "gpio6");
        gpio_request(13, "gpio13");
        gpio_request(19, "gpio19");
        gpio_request(26, "gpio26");
        gpio_request(16, "gpio16");
        gpio_request(20, "gpio20");
        gpio_request(21, "gpio21");

        gpio_direction_input(5);
        gpio_direction_input(6);
        gpio_direction_input(13);
        gpio_direction_input(19);
        gpio_direction_input(26);
        gpio_direction_input(16);
        gpio_direction_input(20);
        gpio_direction_input(21);

        gpio_ctr = ioremap(GPIO_BASE, 0x1000);

        set_gpio_pullup(gpio_ctr, 5);
        set_gpio_pullup(gpio_ctr, 6);
        set_gpio_pullup(gpio_ctr, 13);
        set_gpio_pullup(gpio_ctr, 19);
        set_gpio_pullup(gpio_ctr, 26);
        set_gpio_pullup(gpio_ctr, 16);
        set_gpio_pullup(gpio_ctr, 20);
        set_gpio_pullup(gpio_ctr, 21);

        strcpy(gpiobuf, "");

        ret = request_irq(gpio_to_irq(5), gpio5_irq, IRQF_TRIGGER_FALLING, "rpigpio5", NULL);
        if (ret < 0)
        {
                pr_alert("%s: request_irq failed with %d\n", __func__, ret);
        }
        ret = request_irq(gpio_to_irq(6), gpio6_irq, IRQF_TRIGGER_FALLING, "rpigpio6", NULL);
        if (ret < 0)
        {
                pr_alert("%s: request_irq failed with %d\n", __func__, ret);
        }
        ret = request_irq(gpio_to_irq(13), gpio13_irq, IRQF_TRIGGER_FALLING, "rpigpio13", NULL);
        if (ret < 0)
        {
                pr_alert("%s: request_irq failed with %d\n", __func__, ret);
        }
        ret = request_irq(gpio_to_irq(19), gpio19_irq, IRQF_TRIGGER_FALLING, "rpigpio19", NULL);
        if (ret < 0)
        {
                pr_alert("%s: request_irq failed with %d\n", __func__, ret);
        }
        ret = request_irq(gpio_to_irq(26), gpio26_irq, IRQF_TRIGGER_FALLING, "rpigpio26", NULL);
        if (ret < 0)
        {
                pr_alert("%s: request_irq failed with %d\n", __func__, ret);
        }
        ret = request_irq(gpio_to_irq(16), gpio16_irq, IRQF_TRIGGER_FALLING, "rpigpio16", NULL);
        if (ret < 0)
        {
                pr_alert("%s: request_irq failed with %d\n", __func__, ret);
        }
        ret = request_irq(gpio_to_irq(20), gpio20_irq, IRQF_TRIGGER_FALLING, "rpigpio20", NULL);
        if (ret < 0)
        {
                pr_alert("%s: request_irq failed with %d\n", __func__, ret);
        }
        ret = request_irq(gpio_to_irq(21), gpio21_irq, IRQF_TRIGGER_FALLING, "rpigpio21", NULL);
        if (ret < 0)
        {
                pr_alert("%s: request_irq failed with %d\n", __func__, ret);
        }

        pr_info("rpigpio module installed\n");
        return 0;
}

static void __exit rpigpio_exit(void)
{
        free_irq(gpio_to_irq(5), NULL);
        free_irq(gpio_to_irq(6), NULL);
        free_irq(gpio_to_irq(13), NULL);
        free_irq(gpio_to_irq(19), NULL);
        free_irq(gpio_to_irq(26), NULL);
        free_irq(gpio_to_irq(16), NULL);
        free_irq(gpio_to_irq(20), NULL);
        free_irq(gpio_to_irq(21), NULL);

        gpio_free(5);
        gpio_free(6);
        gpio_free(13);
        gpio_free(19);
        gpio_free(26);
        gpio_free(16);
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
