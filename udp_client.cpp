#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <cstring>
#include <string>
#include <Windows.h>
#include <fstream>  // 用于文件操作

// 手动定义CP_GB2312（兼容旧版MinGW）
#ifndef CP_GB2312
#define CP_GB2312 936
#endif

// 仅在Visual Studio环境下使用链接指令
#if defined(_WIN32) && defined(_MSC_VER)
#pragma comment(lib, "ws2_32.lib")
#endif

// 工具函数：手动解析JSON，提取session_id
std::string extractSessionId(const std::string& jsonResponse) {
    std::string key = "\"session_id\": \"";
    size_t keyPos = jsonResponse.find(key);
    if (keyPos == std::string::npos) return "";
    
    size_t valueStart = keyPos + key.length();
    size_t valueEnd = jsonResponse.find("\"", valueStart);
    if (valueEnd == std::string::npos) return "";
    
    return jsonResponse.substr(valueStart, valueEnd - valueStart);
}

class UdpClient {
private:
    SOCKET m_socket;
    WSADATA m_wsaData;
    bool m_initialized;

public:
    UdpClient() : m_socket(INVALID_SOCKET), m_initialized(false) {}
    ~UdpClient() { cleanup(); }

    bool initialize() {
        int result = WSAStartup(MAKEWORD(2, 2), &m_wsaData);
        if (result != 0) {
            std::cerr << "WSAStartup失败: 错误代码 " << result << std::endl;
            return false;
        }

        m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (m_socket == INVALID_SOCKET) {
            std::cerr << "创建套接字失败: 错误代码 " << WSAGetLastError() << std::endl;
            WSACleanup();
            return false;
        }

        m_initialized = true;
        return true;
    }

    bool sendData(const std::string& serverIp, unsigned short port, const std::string& data) {
        if (!m_initialized) {
            std::cerr << "客户端未初始化" << std::endl;
            return false;
        }

        sockaddr_in serverAddr;
        std::memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);

        serverAddr.sin_addr.s_addr = inet_addr(serverIp.c_str());
        if (serverAddr.sin_addr.s_addr == INADDR_NONE) {
            std::cerr << "无效的IP地址: " << serverIp << std::endl;
            return false;
        }

        int sendLen = sendto(m_socket, data.c_str(), data.length(), 0, 
                            reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));
        if (sendLen == SOCKET_ERROR) {
            std::cerr << "发送数据失败: 错误代码 " << WSAGetLastError() << std::endl;
            return false;
        }

        std::cout << "已发送 " << sendLen << " 字节数据: " << data << std::endl;
        return true;
    }

    std::string receiveData(int timeoutMs = 5000) {
        if (!m_initialized) {
            std::cerr << "客户端未初始化" << std::endl;
            return "";
        }

        DWORD timeout = timeoutMs;
        setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, 
                  reinterpret_cast<char*>(&timeout), sizeof(timeout));

        char recvBuf[1024] = {0};
        sockaddr_in fromAddr;
        int fromLen = sizeof(fromAddr);
        
        int recvLen = recvfrom(m_socket, recvBuf, sizeof(recvBuf) - 1, 0,
                              reinterpret_cast<sockaddr*>(&fromAddr), &fromLen);
        
        if (recvLen > 0) {
            std::cout << "接收到 " << recvLen << " 字节数据: " << recvBuf << std::endl;
            return std::string(recvBuf, recvLen);
        } else if (recvLen == 0) {
            std::cout << "连接已关闭" << std::endl;
        } else {
            std::cerr << "接收数据失败或超时: 错误代码 " << WSAGetLastError() << std::endl;
        }
        
        return "";
    }

    void cleanup() {
        if (m_socket != INVALID_SOCKET) {
            closesocket(m_socket);
            m_socket = INVALID_SOCKET;
        }
        
        if (m_initialized) {
            WSACleanup();
            m_initialized = false;
        }
    }
};

int main() {
    UdpClient client;
    if (!client.initialize()) {
        std::cerr << "初始化失败，按任意键退出..." << std::endl;
        std::cin.get();
        return 1;
    }
    
    const std::string serverIp = "117.72.115.162";
    const unsigned short serverPort = 8889;
    const std::string requestData = "{\"type\": \"get_session\", \"student_id\": \"20254673\"}";
    
    if (!client.sendData(serverIp, serverPort, requestData)) {
        client.cleanup();
        std::cerr << "发送失败，按任意键退出..." << std::endl;
        std::cin.get();
        return 1;
    }
    
    std::string response = client.receiveData();
    
    if (!response.empty()) {
        // 提取并显示session_id
        std::string sessionId = extractSessionId(response);
        std::cout << "------------------------" << std::endl;
        if (!sessionId.empty()) {
            std::cout << "获取到session_id: " << sessionId << std::endl;
        } else {
            std::cerr << "解析失败：未找到session_id" << std::endl;
        }
        
        // 将完整响应保存到文件
        std::ofstream outFile("server_response.txt");
        if (outFile.is_open()) {
            outFile << response;
            outFile.close();
            std::cout << "完整响应已保存到 server_response.txt" << std::endl;
            std::cout << "可使用以下命令查看正确中文：" << std::endl;
            std::cout << "chcp 65001 && more server_response.txt" << std::endl;
        } else {
            std::cerr << "无法创建文件保存响应" << std::endl;
        }
        std::cout << "------------------------" << std::endl;
    } else {
        std::cerr << "未接收到服务器响应" << std::endl;
    }
    
    client.cleanup();
    std::cout << "按任意键退出..." << std::endl;
    std::cin.get();
    return 0;
}
    
