#include <stdio.h>
#include <math.h>

char* itoa(int val, int base){
	static char buf[32] = {0};
	int i = 30;
	for(; val && i ; --i, val /= base)
		buf[i] = "0123456789abcdef"[val % base];
	return &buf[i+1];
}

int read_int(int min, int max, char* prompt, char* err_msg, int* def) {
	// TODO: allow default return
	printf(prompt);
	int in;
	scanf("%d", &in);
	return in;
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
int port_init(int port, int wordlen, int stoplen, int parity_check, int speed) {
	/**
	 * 5 00 0
	 * 6 01 1
	 * 7 10 2
	 * 8 11 3
	 */
	printf("wordlen:\n");	
	for (int i = 5; i < 9; i++) {
		printf("%d bit - %s\n", i, itoa(i - 5, 2));
	}
	printf("\n");

	printf("stoplen:\n");
	for (int i = 1; i < 3; i++) {
		printf("%d bit - %s\n", i, itoa(i - 1, 2));
	}
	printf("\n");

	printf("parity:\n");
	for (int i = 0; i < 3; i++) {
		if (i == 0) {
			printf("no parity check");// 00 10
		} else if (i == 1) {
			printf("parity check");// 11
		} else if (i == 2) {
			printf("odd check");// 01
		}
		int i_ = i == 0 ? i//no check 00 10
		       : i == 1 ? 0b11 //even
					 : /*i == 2 ?*/ 0b01;// odd
		printf(": - %s\n", itoa(i_, 2));
	}
	printf("\n");

	printf("speed:\n");
	printf("100 bit/s - %s\n", itoa(0b000, 2));
	for (int i = 150; i <= 9600; i *= 2) {
		printf("%d bit/s - %s\n", i, itoa(log2l(i / 150) + 1, 2));
	}

	// char al = 0b0;
	// al &= wordlen - 5;
	// al &= stoplen - 1;

	// int params;
	// int result;
	// __asm__(
	// 	"mov ah, 00h\n\t"
	// 	"mov dx, %0\n\t"
	// 	"mov al, %1\n\t"
	// 	"int 14h\n\t"
	// 	: "=r" (result)
	// 	: "r" (port, params)
	// );
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
	printf("2. Send message\n");
	printf("3. Receive message\n");
	printf("4. Port state\n");
	printf("5. Exit\n");

	return read_int(1, 5, "\0", bad_input, NULL);
}

int main() {
	int choice;
	do {
		choice = read_choice();
		switch (choice) {
			case 1: {// init port
				int port         = read_port();
				int wordlen      = read_wordlen();
				int stoplen      = read_stoplen();
				int parity_check = read_parity_check();
				int speed        = read_speed();
				port_init(port, wordlen, stoplen, parity_check, speed);
				break;
			}
			case 2:// send msg
			case 3:// read msg
			case 4:// port state
			// case 5: // break;
			default: break; // never happens cause read_int should only return values >1 and <5
		}
	} while (choice != 5);
}