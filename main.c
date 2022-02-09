#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

char* read_string(FILE* fp, size_t size){
	char* str;
	str = realloc(NULL, sizeof(*str) * size);
	if (!str) {
		return str;
	}

	size_t len = 0;
	int ch;
	while ((ch = fgetc(fp)) != EOF && ch != '\n') {
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

char* itoa(int val, int base){
	static char buf[32] = {0};
	int i = 30;
	for(; val && i ; --i, val /= base)
		buf[i] = "0123456789abcdef"[val % base];
	return strdup(&buf[i+1]);
}

void print_enums() {
	printf("wordlen:\n");
	printf("%s - %s\n", "5 bits", itoa(WORDLEN_5bits, 2));
	printf("%s - %s\n", "6 bits", itoa(WORDLEN_6bits, 2));
	printf("%s - %s\n", "7 bits", itoa(WORDLEN_7bits, 2));
	printf("%s - %s\n", "8 bits", itoa(WORDLEN_8bits, 2));
	printf("\n");

	printf("stoplen:\n");
	printf("%s - %s\n", "1 bit", itoa(STOPLEN_1bit , 2));
	printf("%s - %s\n", "2 bits", itoa(STOPLEN_2bits, 2));
	printf("\n");

	printf("%s - %s\n", "NONE" , itoa(PARITY_CHECK_NONE , 2));
	printf("%s - %s\n", "NONE_", itoa(PARITY_CHECK_NONE_, 2));
	printf("%s - %s\n", "ODD"  , itoa(PARITY_CHECK_ODD  , 2));
	printf("%s - %s\n", "EVEN" , itoa(PARITY_CHECK_EVEN , 2));
	printf("\n");
		
	printf("%s - %s\n", "100bps ", itoa(SPEED_100bps , 2));
	printf("%s - %s\n", "150bps ", itoa(SPEED_150bps , 2));
	printf("%s - %s\n", "300bps ", itoa(SPEED_300bps , 2));
	printf("%s - %s\n", "600bps ", itoa(SPEED_600bps , 2));
	printf("%s - %s\n", "1200bps", itoa(SPEED_1200bps, 2));
	printf("%s - %s\n", "2400bps", itoa(SPEED_2400bps, 2));
	printf("%s - %s\n", "4800bps", itoa(SPEED_4800bps, 2));
	printf("%s - %s\n", "9600bps", itoa(SPEED_9600bps, 2));
	printf("\n");
}

int read_int(int min, int max, char* prompt, char* err_msg, int* def) {
	// TODO: allow default return + validation
	printf(prompt);
	int in;
	scanf("%d", &in);
	return in;
}

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

char* bad_input = "Bad input please try again";

int read_port() {
	int def_port = 1;
	return read_int(1, 4, "choose port(1-4, default: 1)", bad_input, &def_port);
}

int read_wordlen() {
	int def_wordlen = 8;
	return read_int(5, 8, "choose word length in bits(5-8, default: 8)", bad_input, &def_wordlen);
}

int read_stoplen() {
	int def_stoplen = 2;
	return read_int(1, 2, "choose amount of stop bits(1-2, default: 2)", bad_input, &def_stoplen);
}

int read_parity_check() {
	int def_parity_check = 1;
	printf("Choose parity check:\n");
	printf("0. No parity check\n");
	printf("1. Check for parity\n");
	printf("2. Check for odd\n");
	printf("default: 1\n");
	return read_int(0, 2, "\0", bad_input, &def_parity_check);
}

int read_speed() {
	int def_speed = 1;
	printf("Choose speed:\n");
	printf("1. 100 bit/s\n");
	for (int i = 2; i < 9; i++) {
		int j = 150 * pow(2, i - 2);
		printf("%d. %d bit/s\n", i, j);
	}
	return read_int(1, 8, "\0", bad_input, &def_speed);
}

int read_choice() {
	printf("1. Initialize port\n");
	printf("2. Initialize port default\n");
	printf("\t wordlen = 8bits\n");
	printf("\t stoplen = 2bits\n");
	printf("\t parity check = even\n");
	printf("\t speed = 4800bps\n");

	printf("3. Send message\n");
	printf("4. Receive message\n");
	printf("5. Port state\n");
	printf("6. Exit\n");

	return read_int(1, 5, "\0", bad_input, NULL);
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

int main() {
	int choice;
	do {
		choice = read_choice();
		switch (choice) {
			case 1: {// init port
				int          port         = read_port();
		    WORDLEN      wordlen      = read_wordlen();
				STOPLEN      stoplen      = read_stoplen();
				PARITY_CHECK parity_check = read_parity_check();
				SPEED        speed        = read_speed();
				port_init(port, wordlen, stoplen, parity_check, speed);
				break;
			} 
			case 2: {// init port default
				int port = read_port();
				port_init(port, WORDLEN_8bits, STOPLEN_2bits, PARITY_CHECK_EVEN, SPEED_4800bps);
				break;
			}
			case 3: {// send msg
				char* msg = read_string(stdin, 16);
				send_string(msg);
				break;
			}
			case 4: {// receive msg
				char* msg = receive_string(16);
				printf(msg);
				break;
			}
			case 5: {// port state
				// TODO: get port state
				// TODO: print port state
				break;
			}
			// case 6: // break;
			default: break; // never happens cause read_int should only return values >1 and <5
		}
	} while (choice != 5);
}