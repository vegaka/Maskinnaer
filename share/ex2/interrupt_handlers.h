#ifndef INTERRUPT_HANDLERS_H
#define INTERRUPT_HANDLERS_H

#define SAMPLES_PER_NOTE 7250
#define MELODY_NOTES 23

uint32_t counter = 0;
uint32_t phaseLeft = 0;
//uint32_t phaseRight = 0;

// Initialize counters to values that prevents them from playing at startup
uint32_t hit_counter = 1024;
uint32_t metal_counter = 1024;
uint32_t win_counter = 1024 * 10;
uint32_t melody_counter = SAMPLES_PER_NOTE * MELODY_NOTES;

uint32_t effect_sample = 0;
int play_melody = 0;

uint32_t buttons_pressed = 0;

void __attribute__ ((interrupt)) TIMER1_IRQHandler();
void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler();
void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler();
void handle_GPIO_interrupt();

#endif
