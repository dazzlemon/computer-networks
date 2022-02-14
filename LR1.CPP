#include <conio.h>
#include <iostream.h>
#include <stdlib.h>

typedef enum {
	WORDLEN_5bits = 0,// 0b00
	WORDLEN_6bits = 1,// 0b01
	WORDLEN_7bits = 2,// 0b10
	WORDLEN_8bits = 3 // 0b11
} WORDLEN;

typedef enum {
	STOPLEN_1bit  = 0,
	STOPLEN_2bits = 1
} STOPLEN;

typedef enum {
	PARITY_CHECK_NONE  = 0,// 0b00
	PARITY_CHECK_NONE_ = 2,// 0b10
	PARITY_CHECK_ODD   = 1,// 0b01
	PARITY_CHECK_EVEN  = 3 // 0b11
} PARITY_CHECK;

typedef enum {
	SPEED_100bps  = 0,// 0b000
	SPEED_150bps  = 1,// 0b001
	SPEED_300bps  = 2,// 0b010
	SPEED_600bps  = 3,// 0b011
	SPEED_1200bps = 4,// 0b100
	SPEED_2400bps = 5,// 0b101
	SPEED_4800bps = 6,// 0b110
	SPEED_9600bps = 7,// 0b111
} SPEED;

int port_status();

void send_byte(unsigned short port, char byte) {
	char res;
	do {
		asm {
			push dx
			push ax

			mov dx, port
			mov ah, 1
			mov al, byte
			int 14h
			mov res, ah

			pop ax
			pop dx
		}
	} while ((res >> 7) & 1);
}

char receive_byte(unsigned short port) {
	char err;
	char byte_;
	do {
		asm {
			push dx
			push ax

			mov dx, port
			mov ah, 2
			int 14h
			mov err, ah

			mov byte_, al

			pop ax
			pop dx
		}
	} while (err);
	return byte_;
}


int port_init(unsigned short port, WORDLEN wordlen, STOPLEN stoplen, PARITY_CHECK parity_check, SPEED speed) {
	// ah would be 0 anyways
	unsigned short params = wordlen
	                      | stoplen      << 2
	                      | parity_check << 3
	                      | speed        << 5;

	// params = speed
	//        | parity_check << 3
	// 			  | stoplen      << 5
	// 			  | wordlen      << 6;
	// params = reverse16(params);

	unsigned short result;
	asm {
		push ax
		push dx

		mov dx, port
		mov ax, params
		int 14h
		mov result, ax

		pop ax
		pop dx
	}
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

int main(int argc, char* argv[]) {
	unsigned short port = atoi(argv[1]);// 0 - com1, 1 - com2, 2 - com3, 3 - com4
	int choice;
	do {
		cout << "1. init\n";
		cout << "2. send\n";
		cout << "3. receive\n";
		cout << "4. end\n";
		cin >> choice;
		switch (choice) {
			case 1:
				port_init(port, WORDLEN_8bits, STOPLEN_2bits, PARITY_CHECK_EVEN, SPEED_4800bps);
				break;
			case 2:
				char byte;
				cout << "input byte: ";
				cin >> byte;
				send_byte(port, byte);
				break;
			case 3:
				cout << receive_byte(port) << '\n';
				break;
			default: break;
		}
	} while (choice != 4);
    return 0;
}