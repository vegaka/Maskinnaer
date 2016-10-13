#ifndef SOUNDS_H
#define SOUNDS_H

/* Notes with their corresponding frequencies (adjusted slightly) */
#define G2 98
#define A2 110
#define B2 123
#define C3 130
#define D3 147
#define E3 165
#define F3 175
#define G3 196
#define A3 220
#define B3 247
#define C4 262
#define D4 294
#define E4 330
#define F4 349
#define G4 392
#define A4 440
#define B4 494
#define C5 523
#define D5 587
#define E5 664
#define F5 698
#define G5 784

/* Array of notes that give the start up sound for the left ear */
int startup_melody[23] = {
	G4, G4, C5, C5, C5, D5, E5, E5, D5, D5, C5, C5, B4, B4, A4, A4, A4, B4,
	    C5, C5,
	G4, G4, G4
};

#endif
