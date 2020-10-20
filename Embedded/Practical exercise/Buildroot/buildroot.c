#define PERIPHERAL_BASE 0x3F000000
#define GPIO_BASE (PERIPHERAL_BASE + 0x200000)

#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

void set_gpio_output_value(void *gpio_ctr, int gpio_nr, int value)
{
	int reg_id = gpio_nr / 32;
	int pos = gpio_nr % 32;
	if(value){
		#define GPIO_SET_OFFSET 0x1c
		unsigned int* output_set = (unsigned int*) (gpio_ctr + GPIO_SET_OFFSET + 0x4 * reg_id);
		*output_set = 0x1 << pos;
	}
	else{
		#define GPIO_CLR_OFFSET 0x28
		unsigned int* output_clr = (unsigned int*) (gpio_ctr + GPIO_CLR_OFFSET + 0x4 * reg_id);
		*output_clr = 0x1 << pos;
	}
}

void set_gpio_output(void *gpio_ctr, int gpio_nr)
{
	int reg_id = gpio_nr / 10;
	int pos = gpio_nr % 10;
	unsigned int* fsel_reg = (unsigned int*) (gpio_ctr + 0x4 * reg_id);
	unsigned int fsel_val = *fsel_reg;
	unsigned int mask = 0x7 << (pos * 3);
	fsel_val = fsel_val & ~mask;
	unsigned int gpio_output_select = 0x1 << (pos * 3);
	fsel_val = fsel_val | gpio_output_select;
	*fsel_reg = fsel_val;
}

void set_gpio_input(void *gpio_ctr, int gpio_nr)
{
	int reg_id = gpio_nr / 10;
	int pos = gpio_nr % 10;
	unsigned int* fsel_reg = (unsigned int*) (gpio_ctr + 0x4 * reg_id);
	unsigned int fsel_val = *fsel_reg;
	unsigned int mask = 0x7 << (pos * 3);
	fsel_val = fsel_val & ~mask;
	*fsel_reg = fsel_val;
}

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
	usleep(1);
	*pudclk_reg = (0x1 << pos);
	usleep(1);
	*pud_reg = 0;
	*pudclk_reg = 0;
}

void get_gpio_input_value(void *gpio_ctr, int gpio_nr, int *value)
{
	int reg_id = gpio_nr / 32;
	int pos = gpio_nr % 32;
	#define GPIO_LEV_OFFSET 0x34
	unsigned int* level_reg = (unsigned int*) (gpio_ctr + GPIO_LEV_OFFSET + 0x4 * reg_id);
	unsigned int level = *level_reg & (0x1 << pos);
	*value = level? 1:0;
}

int main() 
{
	int fdmem = open("/dev/mem",O_RDWR);
	if (fdmem<0){
	       	printf("Error opening /dev/mem");
	       	return -1;
       	}
	void* gpio_ctr = mmap(0, 4096, PROT_READ+PROT_WRITE, MAP_SHARED, fdmem, GPIO_BASE);
	if(gpio_ctr==MAP_FAILED){
	       	printf("mmap error ");
	       	return -1; 
	}
	set_gpio_output(gpio_ctr,13);
	set_gpio_output(gpio_ctr,19);
	set_gpio_output(gpio_ctr,26);
	
	set_gpio_output_value(gpio_ctr,13,0); 
	set_gpio_output_value(gpio_ctr,19,0); 
	set_gpio_output_value(gpio_ctr,26,0); // 프로그램 시작시 LED OFF

	while(1)
	{
        set_gpio_output_value(gpio_ctr,13,0); 
        set_gpio_output_value(gpio_ctr,19,0); 
        set_gpio_output_value(gpio_ctr,26,1); // LED R
        sleep(1);
        set_gpio_output_value(gpio_ctr,13,0);
        set_gpio_output_value(gpio_ctr,19,1);
        set_gpio_output_value(gpio_ctr,26,0); // LED G
        sleep(1);                
        set_gpio_output_value(gpio_ctr,13,1);
        set_gpio_output_value(gpio_ctr,19,0);
        set_gpio_output_value(gpio_ctr,26,0); // LED B
        sleep(1);
	}
	munmap(gpio_ctr,4096);
	close(fdmem);
}
