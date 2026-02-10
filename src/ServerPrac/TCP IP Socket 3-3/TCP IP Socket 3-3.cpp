#include <iostream>
#include <winsock2.h>
#include "Common.h"
#include <ws2tcpip.h>
#include <string>
#include <iomanip>

/*
	오늘은 IP 주소 변환 함수..

	응용 프로그램은 IP 주소를 문자혈 형태로 전달받는다.. (192.168.0.1)
	결국 네트워크 통신을 위해선 이걸 32비트(IPv4) 또는 128비트(IPv6) 숫자로 변환해야 한다.

	이 변환을 위해 사용되는 함수가 IP 주소 변환 함수인 inet_pton() 과 inet_ntop() 이다.

	inet_pton() : 문자열 -> 숫자 변환 함수
	inet_pton(주소_체계, IP_문자열, 주소_멤버_포인터값);

	inet_ntop() : 숫자 -> 문자열 변환 함수
	inet_ntop(주소_체계, 주소_멤버_포인터값, 문자열_버퍼, 버퍼_크기);

	그리고 IPv4/IPv6 주소 둘 다 지원한다.
	물론, 주소 체계 인수로 AF_INET / AF_INET6 를 넣어서 구분해줘야 한다.

	그리고 이게 두 함수가 비슷해보이는데, 실제로는 좀 다르다.
	문자열 = 사람이 쓰는거,
	소켓 주소 구조체 = OS가 쓰는거라서
	사람이 보는 주소 <-> OS가 쓰는 주소 사이를 바꾸는 함수인건데,
	사람 -> OS로 갈 때랑 OS -> 사람으로 갈 때가 당연히 필요한 인수가 다를 수밖에 없다.

	그래서 pton() = 문자열(IP) 넣고 구조체 멤버(바이너리) 채우기
	ntop() = 주소(바이너리) 넣고 사람이 따로 만들어둔 문자열 버퍼에 쓰기.
	그래서 ntop()를 쓸 때는 버퍼와 버퍼의 크기를 넣어줘야한다..

	그리고 inet_pton() 함수에서는 성공/실패를 반환값으로 처리한다.
		1 : 성공
		0 : 주소 문자열 형식이 잘못됨 (오류 코드는 WSAGetLastError()로 확인)
		-1 : af(주소 체계)가 유효하지 않음 (사실 -1은 윈도우에선 보통 잘 안씀)

	이 외에도 IPv4 주소만 지원하는 구형 함수들도 있다.
	inet_addr() : IPv4 전용 문자열 -> 숫자 변환 함수
	inet_ntoa() : IPv4 전용 숫자 -> 문자열 변환 함수
	
	그리고, 윈도우 전용 함수이고 IPv4/IPv6 주소를 모두 지원하는 WSAStringToAddress() 와 WSAAddressToString() 도 있다.
	이 함수들은 좀 나중에 살펴보자.

	이 정도면 될 듯.

	이제 IP 주소 변환 함수들을 사용해서 실습을 해보자.
*/

int main() {

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return 1;
	}

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0); // 익숙해지기 위해서 한번 소켓 열어본것

	if (sock == INVALID_SOCKET) {
		err_display("socket()");
	}

	const char ipv4str[INET_ADDRSTRLEN] = "192.168.0.1"; // INET_ADDRSTRLEN은 IPv4 주소가 들어갈 최대 버퍼 길이를 나타낸다.
	std::cout << "IPv4 주소(문자열) : " << ipv4str << '\n';
	
	struct in_addr ipv4num;
	if (inet_pton(AF_INET, ipv4str, &ipv4num) != 1) { // IP 주소 문자열 -> 숫자로 바꾸기
		std::cout << "IPv4 : inet_pton() 실패" << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	std::cout << std::hex;
	std::cout << "IPv4 주소(숫자로 변환) : 0x" << ntohl(ipv4num.s_addr) << '\n';
	
	std::cout << std::dec;
	char ipv4str2[INET_ADDRSTRLEN]; // INET_ADDSTRLEN으로 주소 공간(버퍼)을 만들어줘야 한다.
	if (!inet_ntop(AF_INET, &ipv4num, ipv4str2, sizeof(ipv4str2))) {
		std::cout << "IPv4 : inet_ntop() 실패 " << "\n";
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	std::cout << "IPv4 주소(다시 문자열로 변환) : " << ipv4str2 << '\n';

	const char ipv6str[INET6_ADDRSTRLEN] = "2001:0db8:85a3:0000:0000:8a2e:0370:7334"; // INET6_ADDRSTRLEN은 IPv6 주소가 들어갈 최대 버퍼 길이를 나타낸다.
	std::cout << "IPv6 주소(문자열) : " << ipv6str << '\n';

	struct in6_addr ipv6num; // IPv6 소켓 주소 구조체
	if (inet_pton(AF_INET6, ipv6str, &ipv6num) != 1) { // IP 주소 문자열 -> 숫자로 바꾸기
		std::cout << "IPv6 : inet_pton() 실패" << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}

	
	std::cout << "IPv6 주소(숫자로 변환) : 0x";
	std::cout << std::hex << std::setfill('0');
	for (int i = 0; i < 16; i++) {
		std::cout << std::setw(2) << (int)(u_char)ipv6num.s6_addr[i];
	}
	std::cout << '\n' << std::dec;

	char ipv6str2[INET6_ADDRSTRLEN]; // 여기도 물론 INET6_ADDSTRLEN으로 주소 공간(버퍼)을 만들어줘야 한다.
	if (!inet_ntop(AF_INET6, &ipv6num, ipv6str2, sizeof(ipv6str2))) {
		std::cout << "IPv6 : inet_ntop() 실패 " << "\n";
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	std::cout << "IPv6 주소(다시 문자열로 변환) : " << ipv6str2 << '\n';

	closesocket(sock);

	WSACleanup();
}