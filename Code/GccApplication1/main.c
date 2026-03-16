#define F_CPU 16000000UL // Assuming 16MHz clock
#include <avr/io.h>
#include <util/delay.h>

void io_init(void) {
	// --- INPUTS (Set DDR bits to 0) ---
	// SW1-SW6 are on PC0-PC5
	DDRC &= ~0x3F;
	// SW7 is on PB4
	DDRB &= ~(1 << PB4);

	// --- OUTPUTS (Set DDR bits to 1) ---
	// RL2 is PB2, RL4 is PB0
	DDRB |= (1 << PB2) | (1 << PB0);
	// RL5, RL6, RL8 are PD5, PD4, PD2
	DDRD |= (1 << PD5) | (1 << PD4) | (1 << PD2);
}

int main(void) {
	io_init();

	// Variable to track if RL8 has already performed its 3s sequence
	uint8_t rl8_done = 0;

	while (1) {
		// 1. Logic for SW2, 3, 4, 5 -> RL4, 5, 6
		// PINC reads the state of Port C
		if ((PINC & (1 << PC1)) && (PINC & (1 << PC2)) &&
		(PINC & (1 << PC3)) && (PINC & (1 << PC4))) {
			
			// Turn RL4, 5, 6 ON
			PORTB |= (1 << PB0);
			PORTD |= (1 << PD5) | (1 << PD4);

			// Handle RL8: High for 3 seconds once
			if (!rl8_done) {
				PORTD |= (1 << PD2);   // RL8 High
				_delay_ms(3000);       // Wait 3s
				PORTD &= ~(1 << PD2);  // RL8 Low
				rl8_done = 1;          // Prevent re-triggering until SWs are released
			}
			} else {
			// Reset RL8 trigger if inputs are released
			rl8_done = 0;
		}

		// 2. Override: SW7 or SW6 -> RL4, 5, 6 OFF
		// SW6 is PC5, SW7 is PB4
		if ((PINC & (1 << PC5)) || (PINB & (1 << PB4))) {
			PORTB &= ~(1 << PB0);             // RL4 OFF
			PORTD &= ~((1 << PD5) | (1 << PD4)); // RL5, RL6 OFF
		}

		// 3. SW1 -> RL2 Logic
		// SW1 is PC0, RL2 is PB2
		if (PINC & (1 << PC0)) {
			PORTB |= (1 << PB2);  // RL2 ON
			} else {
			PORTB &= ~(1 << PB2); // RL2 OFF
		}
	}
}