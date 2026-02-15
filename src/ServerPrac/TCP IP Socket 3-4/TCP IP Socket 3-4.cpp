#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <winsock2.h>
#include "Common.h"
#include <ws2tcpip.h>


#define TESTNAME "www.google.com"

/*
	오늘은 DNS와 이름 변환 함수

	도메인 네임은 사람이 읽을 수 있는 형태의 주소 체계이다. (예 : www.google.com)
	물론 그냥 IP 주소로 접속해도 되지만, 사람이 IP 주소를 외우기 어렵기 때문에 도메인 네임이 사용된다.

	근데 네트워크 통신을 하려면 당연히 사용자가 입력한 도메인 네임을 숫자(바이너리) 형태의 IP 주소로 변환해야한다..

	도메인 네임과 IP 주소의 변환 정보는 인터넷에 존재하는 여러 DNS(Domain Name System) 서버가 관리한다.
	한 DNS 서버가 모든 도메인 네임과 IP 주소의 변환 정보를 관리하는건 아니다.
	어떻게 보면 분산 DB?

	DNS는 나중에 네트워크 더 공부할 때 살펴보자..

	일단 이 책에서는 DNS가 정상적으로 작동한다고 가정하고 진행한다고 한다.

	뭐,일단.
	응용 프로그램이 도메인 네임 <-> IP 주소 간 변환을 할 수 있도록 다음과 같은 소켓 함수가 제공된다.

	gethostbyname() : 도메인 네임 -> IPv4 주소 변환 함수
	gethostbyname(const char *도메인_네임);

	gethostbyaddr() : IPv4 주소 -> 도메인 네임 변환 함수
	gethostbyaddr(const char *바이너리_IP_주소_포인터, int 주소_길이,int 주소_체계);

	이 두 함수는 구식이라 요즘은 getaddrinfo()/getnameinfo() 함수를 사용한다.
	일단 이번에는 책에 나와있는 대로 구식 함수인 gethostbyname()/gethostbyaddr() 함수를 사용해서 실습을 해봤다.'

	getaddrinfo()/getnameinfo()는 좀 이따 알아보자.

	그리고 이 두 구식 함수는 모두 hostent 구조체를 반환한다.

	struct hostent {
		char *공식_도메인_네임;
		char **별명_목록(aliase name)(참조 변수 느낌. 한 호스트의 도메인 네임이 여러개일 때.);
		short 주소_체계(AF_INET/AF_INET6);
		short 주소_길이(바이트 단위, 4(IPv4)/16(IPv6));
		char **주소_목록(네트워크 바이트 정렬. 한 호스트가 IP 주소를 여러개 가지고 있으면 이 포인터를 따라가면 모든 ip 주소를 얻을 수 있다.);
	}

	뭐, 이 정도면 될 듯.
*/

// 도메인 네임을 입력받아 해당 도메인의 IPv4 주소를 얻는 함수
// gethostbyname() 함수 사용
bool GetIPAddr(const char* name, struct in_addr* addr) { // name : 도메인 네임, addr : IPv4 주소를 저장할 in_addr 구조체 포인터

	struct hostent* addr_ptr = gethostbyname(name); // 도메인 네임 -> hostent 구조체 포인터
	if (addr_ptr == NULL) { // 실패하면(ptr 값이 NULL)
		err_display("gethostbyname()"); // 오류 출력
		return false; // false 반환
	}
	if (addr_ptr->h_addrtype != AF_INET || addr_ptr->h_addr_list[0] == NULL) { // 주소 체계가 IPv4가 아니거나 주소 목록이 비어있으면
		return false; // false 반환
	}
	memcpy(addr,addr_ptr->h_addr_list[0], addr_ptr->h_length); // addr 포인터가 가리키는 in_addr 구조체에 IPv4 주소 복사
	return true; // true 반환
}

// IPv4 주소를 입력받아 해당 주소의 도메인 네임을 얻는 함수
// gethostbyaddr() 함수 사용
bool GetDomainName(struct in_addr addr, char* name, int namelen) { // addr : IPv4 주소, name : 도메인 네임을 저장할 문자열 버퍼, namelen : 문자열 버퍼의 크기

	struct hostent* dns_ptr = gethostbyaddr((const char*)&addr, sizeof(addr), AF_INET); //  IPv4 주소 -> hostent 구조체 포인터
	if (dns_ptr == NULL) { // 실패하면(ptr 값이 NULL)
		err_display("gethostbyaddr()"); // 오류 출력
		return false; // false 반환
	}
	if (dns_ptr->h_addrtype != AF_INET) { // 주소 체계가 IPv4가 아니면
		return false; // false 반환
	}
	strncpy(name, dns_ptr->h_name, namelen); // name 버퍼에 도메인 네임 복사
	return true; // true 반환
}

/*
	추가로, 유효한 도메인 네임을 IP 주소로 변환하는 일은 가능하지만서도,
	유효한 IP 주소를 도메인 네임으로 변환하는건 불가능할 수도 있다.
		
	왜냐면, 도메인 네임과 IP 주소의 변환 정보는 DNS 서버에 저장되는데, 
	모든 IP 주소에 대해 도메인 네임이 존재하는건 아니기 때문이다.
	(특히 개인용 IP 주소라면 더더욱.)

	마지막으로, hostent 구조체는 스레드당 하나씩 할당된다.
	그러니까, gethostbyname()/gethostbyaddr() 함수가 반환하는 hostent 구조체는
	내부 정적/공유 버퍼를 가리킬 수 있어서
	다음 호출로 내용이 덮어써질 수 있다.
	그래서 변환된 IP 주소나 도메인 네임을 계속 사용하려면 
	다른 소켓 함수를 호출하기 전에 꼭 복사해 놓아야 한다.

	그래서 GetIPAddr()/GetDomainName() 함수에서는
	복사본을 반환하도록 했다..

	getaddrinfo()/getnameinfo() 함수도 조금만 알아보자.

	getaddrinfo() 함수 : 도메인 네임 -> IP 주소 변환 함수
	getaddrinfo(const char *도메인_네임, const char *서비스_이름, const struct addrinfo *힌트, struct addrinfo **결과_리스트);

	getnameinfo() 함수 : IP 주소 -> 도메인 네임 변환 함수
	getnameinfo(const struct sockaddr *소켓_주소_구조체_포인터, int 소켓_주소_구조체_크기, char *호스트_버퍼, int 호스트_버퍼_크기, char *서비스_버퍼, int 서비스_버퍼_크기, int 플래그);

	이건 나중에 직접 쓰면서 알아보자.. 지금은 체력이 딸린다..
*/

int main(int argc, char *argv[]) {

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		std::cout << "[Error] 윈속 초기화 실패\n";
		return 1;
	}

	std::cout << "[Notice] 윈속 초기화 성공\n";

	std::cout << "도메인 네임 : " << TESTNAME << '\n';
	struct in_addr addr;
	if (GetIPAddr(TESTNAME, &addr)) {
		char str[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &addr, str, sizeof(str));
		std::cout << "IPv4 주소 : " << str << '\n';

		char domainName[256];
		if (GetDomainName(addr, domainName, sizeof(domainName))) {
			std::cout << "도메인 네임(다시 변환 후) : " << domainName << '\n';
		}
	}

	WSACleanup();
	return 0;
}