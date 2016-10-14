#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

/* 
 *  Function to set up GPIO mode and interrupts.
 *
 *  Arguments: None
 *  Returns: Void
 */
void setup_gpio()
{
	/* Enable GPIO clock */
	*CMU_HFPERCLKEN0 |= CMU2_HFPERCLKEN0_GPIO;

	/* Set pins 0-7 of port C to input */ 
	*GPIO_PC_MODEL = 0x33333333;

	/* Enable internal pull-up */
	*GPIO_PC_DOUT = 0xff;

#ifndef BASELINE
	/* Enable GPIO interrupts on button down and up */
	*GPIO_EXTIPSELL = 0x22222222;
	*GPIO_EXTIRISE = 0xff;
	*GPIO_EXTIFALL = 0xff;
	*GPIO_IEN = 0xff;
#endif
}
