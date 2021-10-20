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
    
    sockaddr_in client;
    int cliensize = sizeof(client);

    SOCKET clientsocket = accept(listening, (sockaddr*)&client, &cliensize);

    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    memset(host, 0, NI_MAXHOST);
    memset(service, 0, NI_MAXSERV);

    if(getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0){
        cout << host << "Connected on port!" << service << endl;
    }
    else{
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        cout << host << "connected on port!" << service <<
            ntohs(client.sin_port) << endl;
    }

    closesocket(listening);

    char buf[4096];
    while(true){
        memset(buf, 0, 4096);
        int bytesrecieved = recv(clientsocket, buf, 4096, 0);
        if(bytesrecieved == SOCKET_ERROR){
            cerr << "Error in recv(). Quitting..." << endl;
            break;
        }

        if(bytesrecieved == 0){
            cout << "client disconnected!" << endl;
            break;
        }

        send(clientsocket, buf, bytesrecieved + 1, 0);

    }


    closesocket(clientsocket);
    WSACleanup();

    return 0;
}
