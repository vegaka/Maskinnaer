#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>

#define DEVICE_NAME "TDT4258 gamepad driver"

int register_irq(int irq_num);

static irqreturn_t gpio_handler(int irq, void *dev_id, struct pt_regs *regs)
{
	printk("irq: %d", irq);

	return IRQ_HANDLED;
}

static int gamepad_probe(struct platform_device *dev) 
{
	int irq_gpio_even;
	int irq_gpio_odd;
	int irq_even_res;
	int irq_odd_res;

	irq_gpio_even = platform_get_irq(dev, 0);
	irq_gpio_odd = platform_get_irq(dev, 1);

	irq_even_res = register_irq(irq_gpio_even);

	printk("even: %d\n", irq_even_res);

	irq_odd_res = register_irq(irq_gpio_odd);

	printk("odd: %d\n", irq_odd_res);

	return 0;
}

static int gamepad_remove(struct platform_device *dev) 
{
	return 0;
}

static const struct of_device_id device_table[] = {
	{ .compatible = "tdt4258", },
	{},
};

MODULE_DEVICE_TABLE(of, device_table);

static struct platform_driver gamepad_driver = {
	.probe = gamepad_probe,
	.remove = gamepad_remove,
	.driver = {
		.name = DEVICE_NAME,
		.owner = THIS_MODULE,
		.of_match_table = device_table,
	},
};

int register_irq(int irq_num)
{
	int result = request_irq(irq_num, (irq_handler_t) gpio_handler, IRQF_TRIGGER_MASK, DEVICE_NAME, &gamepad_driver);
	if (result < 0) {
		printk(KERN_ALERT "%s: request_irq failed with %d\n", __func__, result);
	}

	return result;
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
	/*printk("Hello World, here is your module speaking\n");*/

	return platform_driver_register(&gamepad_driver);
}

/*
 * template_cleanup - function to cleanup this module from kernel space
 *
 * This is the second of two exported functions to handle cleanup this
 * code from a running kernel
 */

static void __exit gamepad_module_cleanup(void)
{
	 printk("Short life for a small module...\n");
}

module_init(gamepad_module_init);
module_exit(gamepad_module_cleanup);

MODULE_DESCRIPTION("TDT4258 gamepad driver module");
MODULE_LICENSE("GPL");

