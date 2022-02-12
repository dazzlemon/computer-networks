#include <conio.h>
#include <iostream.h>

short portNum = 0;
char byteForSending = '0';
char receivedByte = '0';

int port_status();

// int main(int argc, char* argv[]) {
int main() {
		cout << "Port status: " << port_status() << endl;
		getch();
    return 0;
}

int port_status() {
	char state;
	asm {
		push sp
		push bp
		mov ah, 03h
		mov dx, portNum
		int 14h
		mov state, ah
		pop bp
		pop sp
	}
	return portNum;
}