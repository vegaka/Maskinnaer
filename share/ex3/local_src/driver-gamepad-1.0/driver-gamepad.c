#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/signal.h>
#include <asm/io.h>
#include <asm/siginfo.h>
#include <asm/uaccess.h>

#include "efm32gg.h"

#define DEVICE_NAME "TDT4258"

#define IO_REGION_SIZE 0x11c /* GPIO_IFC - GPIO_PA_BASE */
#define GPIO_EVEN_IRQ 17
#define GPIO_ODD_IRQ 18

static struct fasync_struct *fasync;

/*
 *  Sends a SIGIO signal to tell any listeners that a gamepad interrupt has happened.
 *
 *  Arguments: None
 *  Returns: void
 */
static void signal_game(void)
{
	kill_fasync(&fasync, SIGIO, POLL_IN);
}

/*
 *  This function is called whenever a GPIO interrupt happens.
 *
 *  Arguments: TODO
 *  Returns: TODO
 */
irqreturn_t gpio_handler(int irq, void *dev_id, struct pt_regs *regs)
{
	/* Clear interrupt */
	iowrite32(*GPIO_IF, GPIO_IFC);

	signal_game();

	return IRQ_HANDLED;
}

/*
 *  
 *
 *  Arguments: None
 *  Returns: void
 */
static ssize_t gamepad_read(struct file *file, char __user *data, size_t size, loff_t *offset)
{
    	int res;
	short buttons;

	buttons = *GPIO_PC_DIN;
	printk("Buttons: %d\n", buttons);
	res = copy_to_user(data, &buttons, sizeof(short));
    	
    	return size - res;
}

static int gamepad_async(int fd, struct file *filp, int onflag)
{
	return fasync_helper(fd, filp, onflag, &fasync);
}

static struct file_operations driver_fops = {
	.owner = THIS_MODULE,
	.read = gamepad_read,
	.fasync = gamepad_async
};

static dev_t driver_major;
static struct class *driver_class;
static struct cdev *driver_cdev;

static int __init gamepad_module_init(void)
{

	int result; // Used to check result from functions

	struct resource *io_region;
	int irq_request_res;

	io_region = request_mem_region(GPIO_PA_BASE, IO_REGION_SIZE, DEVICE_NAME);

	if (io_region == NULL)
	{
		printk("Unable to allocate IO memory\n");
		return -1;
	}

	ioremap_nocache(io_region->start, IO_REGION_SIZE);

	/* Interrupts */

	irq_request_res = request_irq(GPIO_EVEN_IRQ, (irq_handler_t) gpio_handler, 0, DEVICE_NAME, 0);
	if (irq_request_res < 0) {
		printk(KERN_ALERT "request_irq failed with %d\n", irq_request_res);
	}

	irq_request_res = request_irq(GPIO_ODD_IRQ, (irq_handler_t) gpio_handler, 0, DEVICE_NAME, 0);
	if (irq_request_res < 0) {
		printk(KERN_ALERT "request_irq failed with %d\n", irq_request_res);
	}

	/* Set pins 0-7 of port C to input */ 
	iowrite32(0x33333333, GPIO_PC_MODEL);

	/* Enable internal pull-up */
	iowrite32(0xff, GPIO_PC_DOUT);

	/* Enable GPIO interrupts on button down and up */
	iowrite32(0x22222222, GPIO_EXTIPSELL);
	iowrite32(0xff, GPIO_EXTIRISE);
	iowrite32(0xff, GPIO_EXTIFALL);
	iowrite32(0xff, GPIO_IEN);

	result = alloc_chrdev_region(&driver_major, 0, 1, DEVICE_NAME);
	if (result < 0) {
		printk(KERN_ALERT "alloc_chrdev_region failed.\n");
	}

	driver_cdev = cdev_alloc();
	cdev_init(driver_cdev, &driver_fops);	
	result = cdev_add(driver_cdev, driver_major, 1);
	if (result < 0) {
		printk(KERN_ALERT "Failed to add cdev\n");
	}

	driver_class = class_create(THIS_MODULE, DEVICE_NAME);
	if (!driver_class) {
		printk(KERN_ALERT "Failed to create class\n");
	}

	if (!device_create(driver_class, NULL, driver_major, NULL, DEVICE_NAME)) {
		printk(KERN_ALERT "Failed to create device\n");
	}

	return 0;
}

static void __exit gamepad_module_cleanup(void)
{
	device_destroy(driver_class, driver_major);
	class_destroy(driver_class);
	cdev_del(driver_cdev);
	release_mem_region(GPIO_PA_BASE, IO_REGION_SIZE);
	unregister_chrdev_region(driver_major, 1);
	printk(KERN_DEBUG "Exiting gamepad kernel module\n");
}

module_init(gamepad_module_init);
module_exit(gamepad_module_cleanup);

MODULE_DESCRIPTION("TDT4258 gamepad driver module");
MODULE_LICENSE("GPL");
