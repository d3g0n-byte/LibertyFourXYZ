#include "trace.h"
#include <Windows.h>
#include <stdio.h>

int trace(const char* message, ...) {
#ifdef _DEBUG
	va_list	arg;
	va_start(arg, message);
	vprintf(message, arg);
	printf("\n");
	va_end(arg);
#endif // _DEBUG
	return 0;
}

int error(const char* message, ...) {
//#ifdef _DEBUG
	va_list	arg;
	va_start(arg, message);
	vprintf(message, arg);
	printf("\n");
	va_end(arg);
//#endif // _DEBUG
	return 0;
}

int print_message(const char* message, ...) {
#ifdef _DEBUG
	va_list	arg;
	va_start(arg, message);
	vprintf(message, arg);
	printf("\n");
	va_end(arg);
#endif // _DEBUG
	return 0;
}



