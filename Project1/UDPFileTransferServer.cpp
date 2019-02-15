#include "UDPFileTransferServer.hpp"
#include "Address.hpp"
#include <sstream>
#include <iostream>
#include <filesystem>


Net::UDPServer::UDPServer(Net::Address localAddress) : localAddress(localAddress) {

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


	// Resovle local address and port for server.
    int socketResult = getaddrinfo(nullptr, portStream.str().c_str(), &hints, &result);
    if(socketResult != 0) {

        WSACleanup();
        throw SocketSetupException("getaddrinfo failed: " + socketResult);
    }

	// Create a socket for the server to listen to.
    localSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if(localSocket == INVALID_SOCKET) {

        freeaddrinfo(result);
        int WSAError = WSAGetLastError();
        WSACleanup();
        
        throw SocketSetupException("socket failed: " + WSAError);
    }

	// Bind the socket.
    socketResult = bind(localSocket, result->ai_addr, static_cast<int>(result->ai_addrlen));
    if(socketResult == SOCKET_ERROR) {

        freeaddrinfo(result);
        closesocket(localSocket);
        int WSAError = WSAGetLastError();
        WSACleanup();

        throw SocketSetupException("Failed to bind socket: " + WSAError);
    }


    freeaddrinfo(result);
}



Net::UDPConnection* Net::UDPServer::ListenForConnection() {
    
	// Listen for connections
    if(listen(localSocket, SOMAXCONN) == SOCKET_ERROR) {

        closesocket(localSocket);
        WSACleanup();

        throw ListenException("Listen failed.");
    }

	struct sockaddr_in clientAddr;
	int addressLength = sizeof(clientAddr);

	// Accept socket.
	SOCKET clientSocket = accept(this->localSocket, reinterpret_cast<struct sockaddr*>(&clientAddr), &addressLength);

	// Get client ip address.
	std::string clientIPAddress{ inet_ntoa(clientAddr.sin_addr) };

	// Create an address for the udp connection.
	Net::Address clientAddress{clientIPAddress, localAddress.GetPort()};

	// Create a new connection object.
	Net::UDPConnection* connection = new Net::UDPConnection(clientSocket, clientAddress);

	return connection;
}