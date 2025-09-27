#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <cstring>

#ifndef CP_GB2312
#define CP_GB2312 936
#endif

#pragma comment(lib, "ws2_32.lib")

// 新增：替换已知的TCP中文乱码
std::string fixTcpGarbage(const std::string& garbageStr) {
    std::string result = garbageStr;
    // 替换“TCP杩炴帴鎴愬姛”为“TCP连接成功”
    result.replace(result.find("TCP杩炴帴鎴愬姛"), 8, "TCP连接成功");
    return result;
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup初始化失败" << std::endl;
        return 1;
    }

    char session_id[100] = "a9c3b7ac-5dba-4545-b2c2-904a21997862";

    SOCKET tcpSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (tcpSocket == INVALID_SOCKET) {
        std::cerr << "创建 TCP 套接字失败，错误代码: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    struct sockaddr_in tcpServerAddr;
    tcpServerAddr.sin_family = AF_INET;
    tcpServerAddr.sin_port = htons(9998);
    tcpServerAddr.sin_addr.s_addr = inet_addr("117.72.115.162");
    if (tcpServerAddr.sin_addr.s_addr == INADDR_NONE) {
        std::cerr << "无效的IP地址" << std::endl;
        closesocket(tcpSocket);
        WSACleanup();
        return 1;
    }

    if (connect(tcpSocket, (struct sockaddr*)&tcpServerAddr, sizeof(tcpServerAddr)) == SOCKET_ERROR) {
        std::cerr << "TCP 连接失败，错误代码: " << WSAGetLastError() << std::endl;
        closesocket(tcpSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "TCP 连接成功" << std::endl;

    char tcpSendBuf[200];
    sprintf(tcpSendBuf, "{\"type\": \"tcp_connect\", \"session_id\": \"%s\"}", session_id);
    int tcpSendLen = send(tcpSocket, tcpSendBuf, strlen(tcpSendBuf), 0);
    if (tcpSendLen == SOCKET_ERROR) {
        std::cerr << "TCP 发送失败，错误代码: " << WSAGetLastError() << std::endl;
        closesocket(tcpSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "已发送 TCP 数据: " << tcpSendBuf << std::endl;

    char tcpRecvBuf[1024] = {0};
    int tcpRecvLen;
    while ((tcpRecvLen = recv(tcpSocket, tcpRecvBuf, sizeof(tcpRecvBuf) - 1, 0)) > 0) {
        // 新增：调用乱码替换函数
        std::string recvStr(tcpRecvBuf);
        std::string fixedStr = fixTcpGarbage(recvStr);
        std::cout << "TCP 接收到数据: " << fixedStr << std::endl;  // 显示替换后的结果

        char disconnectConfirm[] = "{\"type\": \"disconnect_confirm\", \"message\": \"客户端确认断开连接\"}";
        send(tcpSocket, disconnectConfirm, strlen(disconnectConfirm), 0);
        std::cout << "已发送断开确认" << std::endl;
        break;
    }

    closesocket(tcpSocket);
    WSACleanup();
    std::cout << "TCP 连接已关闭" << std::endl;
    return 0;
}
