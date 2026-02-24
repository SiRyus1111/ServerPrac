#include <iostream>
#include <winsock2.h>
#include "Common.h"

#pragma comment(lib, "ws2_32.lib")

char* server_ip = (char*)"127.0.0.1"; // 서버 IP 주소 (로컬 호스트)(루프백 주소)
#define SERVER_PORT 9000
#define BUFFER_SIZE 512

int main(int argc, char* argv[]) {
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		std::cout << "[Error] 윈속 초기화 실패\n";
		return 1;
	}

	if (argc > 1) server_ip = argv[1]; // 명령행 인자가 있으면 IP 주소로 사용한다.

	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	sockaddr_in server_addr{};
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

	int connect_result = connect(sock, (sockaddr*)&server_addr, sizeof(server_addr));
	if (connect_result == SOCKET_ERROR) err_quit("connect()");

	char buf[BUFFER_SIZE + 1];
	int len;

	while (true) {
		std::cout << "보낼 메시지 입력 : ";
		std::cin.getline(buf, BUFFER_SIZE);

		len = (int)strlen(buf);
		if (buf[len - 1] == '\n') buf[len - 1] = '\0';
		if (strlen(buf) == 0) break;

		int send_len = send(sock, buf, (int)strlen(buf), 0);
		if (send_len == SOCKET_ERROR) {
			err_display("send()");
			break;
		}

		std::cout << "[Client] " << len << "바이트 보냄\n";

		int recv_len = recv(sock, buf, send_len, MSG_WAITALL);
		if (recv_len == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		else if (recv_len == 0) {
			break;
		}

		buf[recv_len] = '\0';

		std::cout << "[Client] " << recv_len << "바이트 받음\n";
		std::cout << "[Server] [echo] " << buf << '\n';

	}

	closesocket(sock);

	WSACleanup();
	return 0;
}