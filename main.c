#include <stdio.h>

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
	int params;
	int result;
	__asm__(
		"mov ah, 00h\n\t"
		"mov dx, %0\n\t"
		"mov al, %1\n\t"
		"int 14h\n\t"
		: "=r" (result)
		: "r" (port, params)
	);
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

int stoplen() {
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
	printf("1. 100");
	printf("2. 150");
	printf("3. 300");
	printf("4. 600");
	printf("5. 1200");
	printf("6. 2400");
	printf("7. 4800");
	printf("8. 9600");
	return read_int(1, 8, "\0",bad_input, &def_speed);
}

int main() {
	int choice;
	do {
		printf("1. Initialize port\n");
		printf("2. Send message\n");
		printf("3. Receive message\n");
		printf("4. Port state\n");
		printf("5. Exit\n");

		choice = read_int(1, 5, "\0", bad_input, NULL);

		switch (choice) {
			case 1:// init port
				int port = read_port();
				int wordlen = read_wordlen();
				int stoplen = read_stoplen();
				int parity_check = read_parity_check();
				int speed = read_speed();

				port_init(port, wordlen, stoplen, parity_check, speed);
			case 2:// send msg
			case 3:// read msg
			case 4:// port state
			// case 5: // break;
			// default never happens cause read_int should only return values >1 and <5
		}
	} while (choice != 5);
}