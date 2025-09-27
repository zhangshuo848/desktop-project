#include <windows.h>
#include <wininet.h>
#include <iostream>
#include <cstring>

#pragma comment(lib, "wininet.lib")

int main() {
    // 设置昵称和学号（替换为实际值）
    char nickname[] = "张硕";
    char studentId[] = "20254673";

    // 1. 初始化Internet会话
    HINTERNET hInternet = InternetOpen(
        "ClientApp",               // 用户代理名称
        INTERNET_OPEN_TYPE_DIRECT, // 直接连接（不使用代理）
        NULL, NULL, 0              // 代理相关参数（不使用）
    );
    if (hInternet == NULL) {
        std::cerr << "InternetOpen 失败，错误代码: " << GetLastError() << std::endl;
        return 1;
    }

    // 2. 连接到HTTP服务器
    HINTERNET hConnect = InternetConnect(
        hInternet,                 // 上一步的会话句柄
        "117.72.115.162",          // 服务器IP地址
        8899,                      // 服务器端口
        NULL, NULL,                // 用户名和密码（不需要）
        INTERNET_SERVICE_HTTP,     // 服务类型（HTTP）
        0, 0                       // 标志和上下文值
    );
    if (hConnect == NULL) {
        std::cerr << "InternetConnect 失败，错误代码: " << GetLastError() << std::endl;
        InternetCloseHandle(hInternet); // 清理资源
        return 1;
    }

    // 3. 打开HTTP请求
    HINTERNET hRequest = HttpOpenRequest(
        hConnect,                  // 上一步的连接句柄
        "POST",                    // 请求方法（POST）
        "/submit",                 // 请求路径
        NULL,                      // HTTP版本（默认）
        NULL,                      // 引用页（无）
        NULL,                      // 可接受的MIME类型（任意）
        INTERNET_FLAG_RELOAD,      // 强制从服务器获取最新数据
        0                          // 上下文值
    );
    if (hRequest == NULL) {
        std::cerr << "HttpOpenRequest 失败，错误代码: " << GetLastError() << std::endl;
        InternetCloseHandle(hConnect);  // 清理资源
        InternetCloseHandle(hInternet);
        return 1;
    }

    // 4. 构造POST数据（JSON格式）
    char postData[200];
    sprintf(postData, "{\"nickname\": \"%s\", \"student_id\": \"%s\"}", nickname, studentId);
    std::cout << "准备发送POST数据: " << postData << std::endl;

    // 5. 发送HTTP请求
    // 设置请求头（指定内容类型为JSON）
    const char* headers = "Content-Type: application/json\r\n";
    BOOL sendResult = HttpSendRequest(
        hRequest,          // 请求句柄
        headers,           // 请求头
        -1,                // 自动计算请求头长度
        postData,          // POST数据
        strlen(postData)   // POST数据长度
    );

    if (!sendResult) {
        std::cerr << "HttpSendRequest 失败，错误代码: " << GetLastError() << std::endl;
    } else {
        std::cout << "HTTP POST 请求发送成功" << std::endl;

        // （可选）接收服务器响应
        char responseBuf[1024] = {0};
        DWORD bytesRead = 0;
        if (InternetReadFile(hRequest, responseBuf, sizeof(responseBuf) - 1, &bytesRead) && bytesRead > 0) {
            std::cout << "服务器响应: " << responseBuf << std::endl;
        }
    }

    // 6. 清理所有资源（按打开的相反顺序关闭）
    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    return 0;
}
    
