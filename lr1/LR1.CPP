// Safonov Danylo pz1911(931)
#include <iostream.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>

short portNumber() {
	short pt;
	clrscr();
	printf("Port number: ");
	scanf("%d", &pt);
	return pt - 1;// when passing it as argument to interruption it is 0 indexed
}

// params
//          D0 D1 |             D2 |        D3 D4 |       D5 D6 D7 
// word len(bits) | stop len(bits) | parity check | transfer speed 
void initialize(char params) {
	char res;    
	short pt = portNumber();

	_asm {
		push ax
		push dx

		mov ah, 00h
		mov dx, pt
		mov al, params
		INT 14h
		mov res, ah
		
		pop dx
		pop ax
	}

	cout << "\n\nPort's state:\n" << res << endl;
	int value = (int)res;
	for (int i = 7; i >= 0; i--) {
		cout << (1 << i) & value;
	}
	getch();
}

void send(char* str) {
	char res, s;
	short pt = portNumber();
	do {
		do {
			s = *str;
			_asm {
				push ax
				push dx

				mov ah, 01h
				mov dx, pt
				mov al, s
				INT 14h
				mov res, ah
				
				pop dx
				pop ax
			}
		} while (res & 0x80);// 10000000 binary => error
	} while (*(str++));        
	cout << "Send finished \n";
}

void recieve(char* str) {
	char err;
	char data;
	short pt = portNumber();

	do {
		do {
			_asm {
				push ax
				push dx

				mov ah, 02h
				mov dx, pt
				INT 14h
				mov err, ah
				mov data, al
				
				pop dx
				pop ax
			}
		} while (err);
		str[i] = data;
	} while (str[i++]);
}

int main() {
	clrscr();
	bool exit = false;
	while (!exit) {
		cout << "Menu \n"
		     << "1.Initilize \port \n"
		     << "2.Send message \n"
		     << "3.Recieve message \n"
		     << "4.Exit\n\n Make your choice=>> ";
		int option;
		cin >> option;
		switch (option) {
			case 1: {
				int speed;
				cout << "Choose speed\n"
				     << "1.100 b/s\n"//  192 = 11000000
				     << "2.150 b/s\n"//  193 = 11000001
				     << "3.300 b/s\n"//  194 = 11000010
				     << "4.600 b/s\n"//  195 = 11000011
				     << "5.1200 b/s\n"// 196 = 11000100
				     << "6.2400 b/s\n"// 197 = 11000101
				     << "7.4800 b/s\n"// 198 = 11000110
				     << "8.9600 b/s\n"// 199 = 11000111
				     << "Your choise - ";
				cin >> speed;
				initialize(speed + 191);
				break;
			}
			case 2: {
				char str[256];
				cout << "Sended message: ";
				gets(str);
				send(str); 
				getch();// wait for newline
				break;
			}
			case 3: {
				char str[256];
				recieve(str);      
				cout << "Recieved string: " << str;
				getch();// wait for newline
				break;
			}
			default: {
				exit = true;
				break;
			}
		}
	}
	return 0;
}