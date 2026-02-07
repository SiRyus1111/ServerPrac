#include <winsock2.h>
#include <windows.h>
#include "Common.h"
#include <iostream>

/*
	윈속 초기화

	WSAStartup(WORD wVersionRequested, LPWSADATA lpWSAData);
		- wVersionRequested : 프로그램이 요구하는 윈속 버전. 
			윈도우7/8.1/10/11은 일반적으로 2.2 버전을 사용한다. 
			MAKEWORD(2, 2) = 윈속 버전 2.2 요청()
			
		- lpWSAData : 윈속 관련 정보를 담을 WSAData 구조체 변수의 주소 전달

		그리고 이 함수의 실행을 성공하기 전에는  Winsock 함수 호출이 불가능하다.
		그래서 이 함수의 실패 원인을 WSAGetLastError()로 보지 않고 반환값으로 실패 여부를 판단하는 방식이 흔하다.
		- 반환값: 0(성공), 0이 아닌 값(실패)

		초기화 성공 시 해당 환경에서 Winsock 사용이 가능해진다.
		
*/
/*
	프로그램을 종료할 때 윈속 종료 함수인 WSACleanup() 함수를 호출해야함. 운영체제에 관련 리소스 반환하는 용도..
*/
/*
	소켓 생성

	SOCKET socket(int af, int type, int protocol);
		- af : 주소 체계 지정 (IPv4 - AF_INET, IPv6 - AF_INET6 등)
		- type : 소켓 타입 지정 (신뢰성O/연결형 = SOCK_STREAM, 신뢰성X/비연결형 = SOCK_DGRAM)
		- protocol : 사용할 프로토콜 지정 (TCP = IPPROTO_TCP, UDP = IPPROTO_UDP, 소켓 타입만으로 프로토콜을 결정 가능해서 대개 0을 넣는다..)


	소켓 닫기

	int closesocket(SOCKET s);
*/
int main(int argc, char *argv) {

	WSADATA wsa; // 윈속 관련 정보를 담을 구조체 변수

	// 윈속 초기화
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		std::cout << "[Error] 윈속 초기화 실패\n";
		return 1;
	}
		
	std::cout << "[Notice] 윈속 초기화 성공\n";

	// 소켓 생성
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock == INVALID_SOCKET)
		err_quit("socket()");
	
	std::cout << "[Notice] 소켓 생성 성공\n";

	//소켓 닫기
	if (closesocket(sock) == SOCKET_ERROR)
		err_quit("closesocket()");

	// 윈속 종료
	WSACleanup();
	return 0;
}