#include "UDPFileTransferClient.hpp"
#include "Address.hpp"
#include <sstream>
#include <iostream>


Net::UDPClient::UDPClient(Net::Address localAddress) : localAddress(localAddress) {

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    struct addrinfo* result = nullptr;
    struct addrinfo hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    hints.ai_flags = AI_PASSIVE;

    std::stringstream portStream;
    portStream << localAddress.GetPort();

    int socketResult = getaddrinfo(nullptr, portStream.str().c_str(), &hints, &result);
    if(socketResult != 0) {

        WSACleanup();
        throw SocketSetupException("getaddrinfo failed: " + socketResult);
    }

    localSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if(localSocket == INVALID_SOCKET) {

        freeaddrinfo(result);
        int WSAError = WSAGetLastError();
        WSACleanup();
        
        throw SocketSetupException("socket failed: " + WSAError);
    }

    socketResult = bind(localSocket, result->ai_addr, (int) result->ai_addrlen);
    if(socketResult == SOCKET_ERROR) {

        freeaddrinfo(result);
        closesocket(localSocket);
        int WSAError = WSAGetLastError();
        WSACleanup();

        throw SocketSetupException("Failed to bind socket: " + WSAError;
    }
    
    freeaddrinfo(result);
}



Net::UDPConnection* Net::UDPClient::ListenForConnection() {
    
    if(listen(localSocket, SOMAXCONN) == SOCKET_ERROR) {

        closesocket(localSocket);
        WSACleanup();

        throw ListenException("listen failed.");
    }

    
}
