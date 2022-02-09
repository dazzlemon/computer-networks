#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

char* read_string(FILE* fp, size_t size){
	char* str = (char*)malloc(sizeof(*str) * size);
	if (!str) {
		return str;
	}

	size_t len = 0;
	int ch;
	while ((ch = getc(fp)) != EOF && ch != '\n') {
		str[len++] = ch;
		if (len == size) {
			size += 16;
			str = (char*)realloc(str, sizeof(*str) * size);
			if (!str) {
				return str;
			}
		}
	}
	str[len++] = '\0';
	return (char*)realloc(str, sizeof(*str)*len);
}

char* itoa(int val, int base){
	static char buf[32] = {0};
	int i = 30;
	for(; val && i ; --i, val /= base)
		buf[i] = "0123456789abcdef"[val % base];
	return strdup(&buf[i+1]);
}

int read_int(int min, int max, char* prompt, char* err_msg, int* def) {
	// TODO: allow default return + validation
	printf(prompt);
	int in;
	scanf("%d", &in);
	getc(stdin);
	return in;
}