#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>

#include <asm/io.h>

#include "efm32gg.h"

#define DEVICE_NAME "TDT4258"

#define IO_REGION_SIZE 0x11c /* GPIO_IFC - GPIO_PA_BASE */

irqreturn_t gpio_handler(int irq, void *dev_id, struct pt_regs *regs)
{
	printk("irq: %d\n", irq);

	/* Clear interrupt */
	iowrite32(*GPIO_IF, GPIO_IFC);

	return IRQ_HANDLED;
}

/*
 * template_init - function to insert this module into kernel space
 *
 * This is the first of two exported functions to handle inserting this
 * code into a running kernel
 *
 * Returns 0 if successfull, otherwise -1
 */

static int __init gamepad_module_init(void)
{

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

	irq_request_res = request_irq(17, (irq_handler_t) gpio_handler, 0, DEVICE_NAME, 0);
	if (irq_request_res < 0) {
		printk(KERN_ALERT "%s: request_irq failed with %d\n", __func__, irq_request_res);
	}

	irq_request_res = request_irq(18, (irq_handler_t) gpio_handler, 0, DEVICE_NAME, 0);
	if (irq_request_res < 0) {
		printk(KERN_ALERT "%s: request_irq failed with %d\n", __func__, irq_request_res);
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

	return 0;
}

/*
 * template_cleanup - function to cleanup this module from kernel space
 *
 * This is the second of two exported functions to handle cleanup this
 * code from a running kernel
 */

static void __exit gamepad_module_cleanup(void)
{
	release_mem_region(GPIO_PA_BASE, IO_REGION_SIZE);
	printk("Short life for a small module...\n");
}

module_init(gamepad_module_init);
module_exit(gamepad_module_cleanup);

MODULE_DESCRIPTION("TDT4258 gamepad driver module");
MODULE_LICENSE("GPL");

