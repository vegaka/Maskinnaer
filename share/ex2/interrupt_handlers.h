#ifndef INTERRUPT_HANDLERS_H
#define INTERRUPT_HANDLERS_H

uint32_t counter = 0;
uint32_t phaseLeft = 0;
uint32_t phaseRight = 0;

uint32_t hit_counter = 0;
uint32_t metal_counter = 0;
uint32_t win_counter = 0;
uint32_t effect_sample = 0;

uint32_t buttons_pressed = 0;

void __attribute__ ((interrupt)) TIMER1_IRQHandler();
void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler();
void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler();
void handle_GPIO_interrupt();

#endif
