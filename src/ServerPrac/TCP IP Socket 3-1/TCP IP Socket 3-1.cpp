#include <winsock2.h>
#include <ws2tcpip.h> // IPv4/IPv6 관련 유틸 (예 : getaddrinfo())
#include "Common.h"
#include <iostream>

/*
	소켓 주소 구조체는 어디랑 통신할지(IP + 포트)를 표준화한 구조체이다.
	왜 그냥 IP, Port 두 개로 안 받냐면,
	주소 체계가 IPv4만 있는게 아니고, 다양해서
	API를 막 connect(ip, port) 이딴 식으로 만들면
	IPv4/IPv6마다 함수가 갈라지거나,
	함수 오버로드 지옥이 나거나,
	확장이 어려워진다.
	그래서 OS가 주소는 구조체로 통째로 넘기라고 표준을 잡은 것이다.

	소켓 주소 구조체의 대표 타입 : sockaddr = 16바이트 크기
	struct sockaddr {
		unsigned short sa_family; // 주소 체계 = IPv4 - AF_INET, IPv6 - AF_INET6 / 뭐, 타입은 구현마다 다를 수 있다(예 : ADDRESS_FAMILY 라던지..)
		char sa_data[14]; // 실제 주소 정보 : 주소 체계에 따라 다름
	};

	// sockaddr 구조체는 그저 대표 타입이고, 실제로는 주소 체계에 따라 더 구체적인 구조체를 사용한다.

	IPv4 소켓 주소 구조체 sockaddr_in = 보통 16바이트 크기
	struct sockaddr_in {
		short sin_family; // 주소 체계 = AF_INET (IPv4) / 요것도 타입은 구현마다 다를 수 있다(예 : ADDRESS_FAMILY, sa_family_t 라던지..)
		unsigned short sin_port; // 포트 번호 (네트워크 바이트 오더)
		struct in_addr sin_addr; // IPv4 주소 (32비트인 in_addr 구조체 타입)
		char sin_zero[8]; // 패딩 (항상 0으로 설정)
	};

	IPv6 소켓 주소 구조체 sockaddr_in6 = 28바이트 크기
	struct sockaddr_in6 {
		short sin6_family; // 주소 체계 : AF_INET6 (IPv6)
		unsigned short sin6_port; // 포트 번호 (네트워크 바이트 오더)
		unsigned long sin6_flowinfo; // 플로우 정보 (일반적으로 0으로 설정)
		struct in6_addr sin6_addr; // IPv6 주소 (128비트인 in6_addr 구조체 타입)
		unsigned long sin6_scope_id; // 스코프 ID (일반적으로 0으로 설정)
	};

	 소켓 주소 구조체는 적어놓은 것처럼 크기가 크고,
	 API 자체가 주소 타입이 여러개이고(포인터 + 길이 형태로 받으면 한 함수 시그티처로 모두 처리 가능),
	 그리고 accept() 처럼 OS가 소켓 주소 구조체의 값을 채워 넣어야 하는 경우도 많기 때문에,
	 포인터로 전달한다.

	 그리고 같이 사용할 주소 체계에 따라 소켓 주소 구조체의 크기가 달라지기 때문에
	 sizeof() 연산자를 사용해서 크기를 같이 전달해야 한다.

	 그리고 소켓 주소 구조체를 사용할 때는
	 이렇게 두 가지 정도가 있다.

	 1. 소켓 함수의 인수로 직접 전달 (예 : bind(), connect(), sendto() 등)
	 2. 소켓 함수의 반환값으로 전달받음 (예 : accept(), recvfrom() 등)

	 뭐, 뻔하지?
*/
int main() {
	return 0;
}