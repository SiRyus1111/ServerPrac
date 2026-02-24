#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include "Common.h"

#pragma comment(lib, "ws2_32.lib")

#define SERVER_PORT 9000
#define BUFFER_SIZE 512

int main(int argc, char* argv[]) {
	WSADATA wsa;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		std::cout << "[Error] 윈속 초기화 실패\n";
		return 1;
	}

	SOCKET server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (server_sock == INVALID_SOCKET) {
		err_quit("socket()");
	}

	sockaddr_in server_addr{};
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = htonl(ADDR_ANY);

	if (bind(server_sock, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) err_quit("bind()");

	if (listen(server_sock, SOMAXCONN) == SOCKET_ERROR) err_quit("listen()");

	SOCKET client_sock;
	sockaddr_in client_addr{};

	int addr_len;
	char buf[BUFFER_SIZE + 1];

	while (true) {

		addr_len = sizeof(client_addr);
		client_sock = accept(server_sock, (sockaddr*)&client_addr, &addr_len);

		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &client_addr.sin_addr, addr, sizeof(addr));
		std::cout << "클라이언트 연결됨 : IP 주소=" << addr << ", 포트 번호=" << ntohs(client_addr.sin_port) << '\n';

		while (true) {

			int recv_len = recv(client_sock, buf, BUFFER_SIZE, 0);
			if (recv_len == SOCKET_ERROR) {
				err_display("recv()");
				break;
			}
			else if (recv_len == 0) {
				break;
			}

			buf[recv_len] = '\0';
			std::cout << addr << ":" << ntohs(client_addr.sin_port) << "로부터 받은 메시지 : " << buf << '\n';

			int send_len = send(client_sock, buf, recv_len, 0);
			if (send_len == SOCKET_ERROR) {
				err_display("send()");
				break;
			}
			std::cout << addr << ':' << ntohs(client_addr.sin_port) << "로 에코 메시지 전송 완료\n";
		}

		closesocket(client_sock);
		std::cout << "클라이언트 연결 종료됨 : IP 주소=" << addr << ", 포트 번호=" << ntohs(client_addr.sin_port) << '\n';
	}

	closesocket(server_sock);
	WSACleanup();

	return 0;
}