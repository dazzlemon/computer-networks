#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "serial_port.h"
#include "util.h"

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
				printf("please input message:\n");
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