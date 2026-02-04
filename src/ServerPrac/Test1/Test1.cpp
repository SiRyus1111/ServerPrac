#include <windows.h>
#include <iostream>
#include "Common.h"
#include <cstdlib>

void err_quit(const char* msg)
{
	LPVOID lpMsgBuf;

	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(char*)&lpMsgBuf,
		0,
		NULL);

	MessageBoxA(NULL, (const char*)lpMsgBuf, msg, MB_ICONERROR); // 에러 창을 띄우고
	LocalFree(lpMsgBuf); // 할당된 메모리를 해제한 후
	exit(1); // 걍 종료시켜버림
}

void err_display(const char* msg)
{
	LPVOID lpMsgBuf;

	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(char*)&lpMsgBuf,
		0,
		NULL);

	std::cout << "[" << msg << "] " << (char*)lpMsgBuf << std::endl; // [오류 코드]오류 메시지 형식으로 출력됨
	LocalFree(lpMsgBuf); // 할당된 메모리 해제
}