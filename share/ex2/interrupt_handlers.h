#ifndef INTERRUPT_HANDLERS_H
#define INTERRUPT_HANDLERS_H

uint32_t counter = 0;
uint32_t phaseLeft = 0;
uint32_t phaseRight = 0;

void __attribute__ ((interrupt)) TIMER1_IRQHandler();
void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler();
void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler();

#endif
