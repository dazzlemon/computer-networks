#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef enum {
	WORDLEN_5bits = 0b00,
	WORDLEN_6bits = 0b01,
	WORDLEN_7bits = 0b10,
	WORDLEN_8bits = 0b11
} WORDLEN;

typedef enum {
	STOPLEN_1bit  = 0b0,
	STOPLEN_2bits = 0b1
} STOPLEN;

typedef enum {
	PARITY_CHECK_NONE  = 0b00,
	PARITY_CHECK_NONE_ = 0b10,
	PARITY_CHECK_ODD   = 0b01,
	PARITY_CHECK_EVEN  = 0b11
} PARITY_CHECK;

typedef enum {
	SPEED_100bps  = 0b000,
	SPEED_150bps  = 0b001,
	SPEED_300bps  = 0b010,
	SPEED_600bps  = 0b011,
	SPEED_1200bps = 0b100,
	SPEED_2400bps = 0b101,
	SPEED_4800bps = 0b110,
	SPEED_9600bps = 0b111
} SPEED;

__uint16_t reverse16(__uint16_t nonreversed ) {
	__uint16_t reversed = 0;
	for (__uint16_t i = 0; i < 16; i++) {
		reversed |= (nonreversed >> (16 - i - 1) & 1) << i;
  }        
	return reversed;
}

/**
 * port = 1-4
 * wordlen = 5-8
 * stoplen = 1-2
 * parityCheck =
 * 		0: no parity check
 * 		1: check for parity
 * 		2: check for odd
 * speed = 100/150/300/600/1200/2400/4800/9600 (bit/s)
 */
int port_init(__uint16_t port, WORDLEN wordlen, STOPLEN stoplen, PARITY_CHECK parity_check, SPEED speed) {
	// ah would be 0 anyways
	__uint16_t params = wordlen
	                  | stoplen      << 2
				            | parity_check << 3
				            | speed        << 5;

	// params = speed
	//        | parity_check << 3
	// 			  | stoplen      << 5
	// 			  | wordlen      << 6;
	// params = reverse16(params);

	__uint16_t result;
	__asm__(
		"mov %1, %%dx\n\t"// port
		"mov %2, %%ax\n\t"// params
		"int $0x14\n\t"
		"mov %%ax, %0\n\t"// result
		: "=r" (result)
		: "r" (port), "r" (params)
		: "%ax", "%dx"
	);
	/**
	 * TODO: destructure result
	 * ah - port state
	 * 		0 timeout, if true other bits dont matter
	 * 		1 offset register empty
	 * 		2 buffer register empty
	 * 		3 BREAK
	 * 		4 sync error
	 * 		5 parity error
	 * 		6 input register overflow
	 * 		7 data is ready
	 * al - modem state
	 * 		0 CTS state change
	 * 		1 DSR state change
	 * 		2 RI  state change
	 * 		3 DCD state change
	 * 		4 CTS state
	 * 		5 DSR state
	 * 		6 RI  state
	 * 		7 DCD state
	 */
	return 0;
}

void send_byte(char byte) {
	char res;
	do {
		__asm__(
			"mov 0, %%dx\n\t"
			"mov 1, %%ah\n\t"
			"mov %1, %%al\n\t"// byte
			"int $0x14\n\t"
			"mov %%ah, %0\n\t"// res
			: "=r" (res)
			: "r" (byte)
			: "%ax", "%dx"
		);
	} while ((res >> 7) & 1);
}

void send_string(char* msg) {
	do {
		send_byte(*msg);
	} while (*(msg++));
}

char receive_byte() {
	char err;
	char byte;
	do {
		__asm__(
			"mov 0, %%dx\n\t"
			"mov 2, %%ah\n\t"
			"int $0x14\n\t"
			"mov %%al, %1\n\t"
			// Error: can't encode register '%ah' in an instruction requiring REX prefix. (mov %%ah, %0)
			"mov %%ah, %%al\n\t"
			"mov %%al, %0\n\t"
			: "=r" (err), "=r" (byte)
			: "r" (NULL)
			: "%ax", "%dx"
		);
	} while (err);
	return byte;
}

char* receive_string(size_t size) {
	char* str;
	str = realloc(NULL, sizeof(*str) * size);
	if (!str) {
		return str;
	}

	size_t len = 0;
	int ch;
	while ((ch = receive_byte())) {
		str[len++] = ch;
		if (len == size) {
			size += 16;
			str = realloc(str, sizeof(*str) * size);
			if (!str) {
				return str;
			}
		}
	}
	str[len++] = '\0';
	return realloc(str, sizeof(*str)*len);
}