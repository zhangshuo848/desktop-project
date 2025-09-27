#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup 失败\n");
        return 1;
    }

    // 创建 UDP 套接字
    SOCKET udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udpSocket == INVALID_SOCKET) {
        printf("创建 UDP 套接字失败\n");
        WSACleanup();
        return 1;
    }

    // 配置服务器地址
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8889);
    inet_pton(AF_INET, "117.72.115.162", &serverAddr.sin_addr);

    // 构造发送的 JSON 数据，这里假设学号为 "2025xxxx"
    char sendBuf[] = "{\"type\": \"get_session\", \"student_id\": \"2025xxxx\"}";
    int sendLen = sendto(udpSocket, sendBuf, sizeof(sendBuf), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (sendLen == SOCKET_ERROR) {
        printf("UDP 发送失败\n");
        closesocket(udpSocket);
        WSACleanup();
        return 1;
    }

    // 接收服务器返回的会话标识符
    char recvBuf[1024] = {0};
    struct sockaddr_in fromAddr;
    int fromLen = sizeof(fromAddr);
    int recvLen = recvfrom(udpSocket, recvBuf, sizeof(recvBuf), 0, (struct sockaddr*)&fromAddr, &fromLen);
    if (recvLen > 0) {
        printf("UDP 接收到会话标识符: %s\n", recvBuf);
        // 这里可以解析出 session_id 并保存，用于后续 TCP 连接
    }

    closesocket(udpSocket);
    // 后续 TCP、HTTP 部分代码...
    WSACleanup();
    return 0;
}
