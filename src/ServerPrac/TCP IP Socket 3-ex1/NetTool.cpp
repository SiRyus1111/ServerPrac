#include <iostream> // 표준 입출력(std::cin, std::cout)
#include <winsock2.h> // 윈속2 API
#include <ws2tcpip.h> // inet_pton(), getaddrinfo() 등 소켓 함수와 구조체
#include <string.h> // 문자열 라이브러리 3대장
#include <string>
#include <cstring>
#include <iomanip>


#pragma comment(lib, "ws2_32.lib")

// ipparse 함수 구현
// ip 주소를 받아, 주소 체계를 출력하고 다시 문자렬로 바꿔 출력하는 함수
// IPv4도 IPv6도 아닌 ip 주소라면 invaild_ip를 출력한다.
int cmd_ipparse(const std::string& ip) { // ip 주소를 문자열 형식으로 받는다

	in_addr ipv4_addr{}; // IPv4 바이너리 주소를 담을 구조체 (0으로 초기화)

	if (inet_pton(AF_INET, ip.c_str(), &ipv4_addr) == 1) { // 주소 체계에 맞지 않는 ip 주소가 입력된다면 inet_pton() 함수는 0을 반환한다. 물론 성공하면 1을 반환한다.

		char ipv4_str[INET_ADDRSTRLEN]{}; // INET_ADDRSTRLEN으로 IPv4 주소가 들어갈 최대 버퍼 크기만큼의 ipv4_str 문자열을 만든다.
		if (inet_ntop(AF_INET, &ipv4_addr, ipv4_str, sizeof(ipv4_str)) == nullptr) { // IPv4 바이너리 주소를 담은 in_addr 구조체의 주소를 다시 문자열로 바꾼다. 물론 실패 예외처리도 함.
			std::cout << "invalid_ip\n";
			return 1;
		}
		
		std::cout << "family=IPv4\n" << "roundtrip=" << ipv4_str << '\n'; // 출력
		return 0; // 함수 종료
	}

	in6_addr ipv6_addr{}; // IPv6 바이너리 주소를 담을 구조체 (0으로 초기화)

	if (inet_pton(AF_INET6, ip.c_str(), &ipv6_addr) == 1) { // 주소 체계에 맞지 않는 ip 주소가 입력된다면 inet_pton() 함수는 0을 반환하고, 성공하면 1을 반환하기 때문에, IPv6 주소임을 판별하기 위해 inet_pton() == 0을 조건으로 넣는다.

		char ipv6_str[INET6_ADDRSTRLEN]{}; // INET6_ADDRSTRLEN으로 IPv6 주소가 들어갈 최대 버퍼 크기만큼의 ipv4_str 문자열을 만든다.
		if (inet_ntop(AF_INET6, &ipv6_addr, ipv6_str, sizeof(ipv6_str)) == nullptr) { // IPv6 바이너리 주소를 담은 in6_addr 구조체의 주소를 다시 문자열로 바꾼다. 물론 실패 예외처리도 함.
			std::cout << "invalid_ip\n";
			return 1;
		}

		std::cout << "family=IPv6\n" << "roundtrip=" << ipv6_str << '\n'; // 출력
		return 0; // 함수 종료
	}

	std::cout << "invalid_ip\n"; // IPv4와 IPv6 주소 둘 다 아니라면 invaild_ip를 출력한다.

	return 1; // 함수 종료
}

// endian 함수 구현
// 포트 번호를 받아 출력 후, 네트워크 바이트 정렬(16진수)로 한 번, 다시 원래대로 호스트 바이트 정렬로 한 번 출력하는 함수.
int cmd_endian(const int& port_num) { // u_short(0~65535) 형식으로 포트 번호를 받는다.

	if (!(port_num <= 65535 && port_num >= 0)) { // 포트 번호는 0~65535 사이의 숫자여야 하므로, 예외처리로 이 범위를 벗어나는 숫자가 입력되면 사용법을 출력하고 함수 종료한다.
		std::cout << "사용법 : endian <uint16_port>\n";
		return 1; // 함수 종료
	}

	u_short port = (u_short) port_num; // 입력받은 포트 번호를 u_short 형식으로 변환한다. 물론 예외처리도 한다.

	std::cout << "host=" << port << '\n'; // 입력받은 포트 번호 출력

	

	u_short net_port_num = htons(port_num); // 포트 번호를 htons() 함수로 네트워크 바이트 정렬로 변환
	u_char* net_port_bytes = (u_char*)&net_port_num;
	std::cout << std::hex << std::setfill('0') << "net=0x";
	for (size_t i = 0; i < sizeof(net_port_num); i++) {
		std::cout << std::setw(2) << (u_short)*(net_port_bytes + i);
	}
	std::cout << '\n';
	// 16진수로 출력

	u_short host_port_num = ntohs(net_port_num); // 포트 번호를 ntohs() 함수로 다시 호스트 바이트 정렬로 변환
	std::cout << std::dec << "back=" << host_port_num << '\n'; // 10진수로 출력

	return 0; // 함수 종료
}


// resolve 함수 구현
// 호스트(도메인 네임/IP 주소) 정보와 서비스(포트 번호/서비스 명) 정보를 받아서 IPv4면 IPv4 주소로, IPv6면 IPv6 주소로 출력하는 함수.
int cmd_resolve(const std::string& host, const std::string& service) { // host와 service를 참조 변수로 받는다. 

	addrinfo hints{}; // addrinfo 구조체 형식의 hints 구조체의 모든 멤버를 0으로 초기화
	hints.ai_family = AF_UNSPEC; // hints 구조체의 주소 체계 필드에 IPv4/IPv6 둘 다 해당하도록 AF_UNSPEC을 대입한다.
	hints.ai_socktype = SOCK_STREAM; // hints 구조체의 소켓 타입 필드에 신뢰성 있는/연결형 통신을 의미하는 SOCK_STREAM을 대입한다. 
	hints.ai_protocol = IPPROTO_TCP; // hints 구조체의 프로토콜 필드에 TCP를 의미하는 IPPROTO_TCP를 대입한다.

	addrinfo* result = nullptr; // addrinfo 구조체 형식의 result 포인터 변수를 nullptr로 초기화한다.

	int rc = getaddrinfo(host.c_str(), service.c_str(), &hints, &result); // getaddrinfo() 함수를 호출하고, 예외 처리를 하기 위해 리턴값을 변수 rc에 저장한다.

	if (rc != 0) { // getaddrinfo() 함수는 성공시 0을 반환하고, 실패시 오류 코드를 반환하므로, rc != 0이라는 조건으로 실패를 판별한다.
		std::cout << "resolve_failed " << rc << '\n';

		return 1; // 함수 종료
	}

	// result 노드부터 쭉 연결 리스트를 따라가도록 for 문을 돌린다.
	for (addrinfo* p = result; p != nullptr; p = p->ai_next) { // for (addrinfo 구조체 포인터 p = result 구조체 포인터 result부터 시작; 만약 p == nullptr(다음 노드가 없으면)이면 종료; p = 현재 p의 다음 노드의 주소)

		char ipstr[INET6_ADDRSTRLEN]{}; // ip주소를 저장할 변수(IPv6의 최대 버퍼 크기로 지정)(전부 0으로 초기화)

		if (p->ai_family == AF_INET) { // 주소 체계가 IPv4면
			auto* ipv4 = (sockaddr_in*)p->ai_addr; // IPv4 소켓 주소 구조체인 sockaddr_in에 포인터로 할당
			if (inet_ntop(AF_INET, &(ipv4->sin_addr), ipstr, sizeof(ipstr)) != nullptr) { // 소켓 주소 구조체의 IP 주소를 문자열로 변환, 실패 예외처리
				std::cout << "IPv4 " << ipstr << '\n'; // IP 주소를 문자열로 출력
			}
			else { 
				std::cout << "resolve_failed" << '\n';

				return 1;
			}
		}

		else if (p->ai_family == AF_INET6) { // 주소 체계가 IPv6면
			auto* ipv6 = (sockaddr_in6*)p->ai_addr; // IPv6 소켓 주소 구조체인 sockaddr_in6에 포인터로 할당
			if (inet_ntop(AF_INET6, &(ipv6->sin6_addr), ipstr, sizeof(ipstr)) != nullptr) { // 소켓 주소 구조체의 IP 주소를 문자열로 변환, 실패 예외처리
				std::cout << "IPv6 " << ipstr << '\n'; // IP 주소를 문자열로 출력
			}
			else {
				std::cout << "resolve_failed" << '\n';

				return 1;
			}
		}
	}

	freeaddrinfo(result); // freeaddrinfo() 함수로 자원 반환

	return 0; // 함수 종료
}

int main(int argc, char* argv[]) { // argv를 사용해보았다.

	if (argc < 2) {
		std::cerr << "사용법 :\n" // 사용법 출력
			<< "NetTool ipparse <ip> : IP 문자열 <-> 바이너리 변환\n"
			<< "NetTool endian <uint16_port> : 바이트 오더 변환\n"
			<< "NetTool resolve <host> <service> : 도메인 네임 해석 결과 출력\n";

		return 1; // main() 함수(프로그램) 종료
	}

	std::ios::sync_with_stdio(false);
	std::cin.tie(nullptr);

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		std::cout << "[Error] 윈속 초기화 실패";
		return 1;
	}
	
	std::string command = argv[1]; // 입력받은 명령어를 문자열 형식으로 저장할 변수 command에 argv[1]로 입력받은 명령어(char[])를 저장한다.

	if (command == "ipparse") { // 입력받은 문자열이 ipparse면
		
		if (argc < 3) {
			std::cout << "사용법 : ipparse <ip>\n";
			WSACleanup();
			return 1;
		}
		std::string ip = argv[2]; // ip 주소를 문자열 형식으로 저장할 변수 ip에 argv[2]로 입력받은 ip 주소를 저장한다.

		cmd_ipparse(ip); // ipparse() 함수 호출해서 주소 체계와 IP 주소 출력
	}
	else if (command == "endian") { // 입력받은 문자열이 endian이면

		if (argc < 3) { // endian 명령어는 포트 번호가 필요하므로, argc가 3보다 작은 경우는 사용법이 잘못된 경우이므로, 사용법을 출력하고 함수 종료한다.
			std::cout << "사용법 : endian <uint16_port>\n";
			WSACleanup();
			return 1;
		}
		
		int port; // 포트 번호를 저장할 변수 port
		try {
			port = std::stod(argv[2]); // argv[2]로 입력받은 포트 번호를 문자열에서 숫자로 변환해서 port 변수에 저장한다. 물론 실패 예외처리도 한다.
		}
		catch (const std::exception& e) { // 정수로 변환할 수 없는 문자열이 입력되면 예외가 발생하므로, 예외처리를 한다.
			std::cout << "사용법 : endian <uint16_port>\n";
			WSACleanup();
			return 1;
		}

		cmd_endian(port); // endian() 함수 호출해서 포트 번호 출력
	}

	else if (command == "resolve") { // 입력받은 문자열이 resolve면

		if (argc < 4) { // resolve 명령어는 호스트 정보와 서비스 정보 둘 다 필요하므로, argc가 4보다 작은 경우는 사용법이 잘못된 경우이므로, 사용법을 출력하고 함수 종료한다.
			std::cout << "사용법 : resolve <host> <service>\n";
			WSACleanup();
			return 1;
		}

		std::string host = argv[2]; // 호스트 정보를 문자열 형식으로 저장할 변수 host에 argv[2]로 입력받은 호스트 정보를 저장한다.
		std::string service = argv[3]; // 서비스 정보를 문자열 형식으로 저장할 변수 service에 argv[3]로 입력받은 서비스 정보를 저장한다.

		cmd_resolve(host, service); // resolve() 함수 호출해서 주소 체계와 IP 주소 출력
	}
	else {
		std::cerr << "사용법 :\n" // 사용법 출력
			<< "NetTool ipparse <ip> : IP 문자열 <-> 바이너리 변환\n"
			<< "NetTool endian <uint16_port> : 바이트 오더 변환\n"
			<< "NetTool resolve <host> <service> : 도메인 네임 해석 결과 출력\n";

		WSACleanup(); // 윈속 종료(자원 반환)

		return 1;
	}
	WSACleanup(); // 윈속 종료(자원 반환)

	return 0; // main() 함수(프로그램) 종료
}