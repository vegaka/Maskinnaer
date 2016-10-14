#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"
#include "timer.h"

/* 
 *  Set up the timer with the supplied period, and enable interrupts.
 *
 *  Arguments: period: The timer period in clock cycles.
 *  Returns: Void
 */
void setup_timer(uint32_t period)
{

	/* Enable clock to timer by setting bit 6 in CMU_HFPERCLKEN0 */
	*CMU_HFPERCLKEN0 = *CMU_HFPERCLKEN0 | (1 << 6);

	/* Write the period to register TIMER1_TOP */
	*TIMER1_TOP = period;

	/* Enable timer interrupt generation by writing 1 to TIMER1_IEN */
	*TIMER1_IEN = 1;

	/* Start the timer by writing 1 to TIMER1_CMD */
	*TIMER1_CMD = 1;
}
