#include <iostream>
#include <ws2tcpip.h>
#include <sstream>
//#include <winsock2.h>

//#pragma comment(lib, "Ws2_32.lib")

using namespace std;

int main()
{
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);

    int wsok = WSAStartup(ver, &wsData);
    if(wsok != 0){
        cerr << "Can't initialize winshok! Quitting.." << endl;
    }

    // creating a socket

    SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
    if(listening == INVALID_SOCKET){
        cerr << "Can't create a Socket! Quitting.." << endl;
    }

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    hint.sin_addr.S_un.S_addr = INADDR_ANY;
    bind(listening, (sockaddr*)&hint, sizeof(hint));
    listen(listening, SOMAXCONN);
    fd_set master;
    FD_ZERO(&master);
    FD_SET(listening, &master);

    while(true){
        fd_set copy = master;
        int socketCount = select(0, &copy, nullptr, nullptr, nullptr);
        for(int i = 0; i < socketCount; i++){
            SOCKET sock = copy.fd_array[i];
            if(sock == listening){
                SOCKET client = accept(listening, nullptr, nullptr);
                FD_SET(client, &master);
                string welcomeMsg = "welcome to the chat room!";
                send(client, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);
            }
            else{
                char buf[4096];
                memset(buf, 0, 4096);
                int bytesin = recv(sock, buf, 4096, 0);
                if(bytesin <= 0){
                    closesocket(sock);
                    FD_CLR(sock, &master);
                }
                else{
                    for(int i = 0; i < master.fd_count; i++){
                        SOCKET outsock = master.fd_array[i];
                        if(outsock != listening && outsock != sock){
                            ostringstream ss;
                            ss << "Socket #" << sock << ":" << buf << "\r\n";
                            string strOut = ss.str();
                            send(outsock, strOut.c_str(), strOut.size() + 1, 0);
                        }
                    }
                }
            }
        }
    }
    WSACleanup();

    return 0;
}
