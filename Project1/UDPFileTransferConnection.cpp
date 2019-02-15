#include "UDPFileTransferConnection.hpp"

Net::UDPConnection::UDPConnection(SOCKET clientSocket, Net::Address connectedAddress): connectedAddress(connectedAddress) {


	if (clientSocket == INVALID_SOCKET) {

		WSACleanup();
		throw  ConnectionException("Invalid socket in connection.");
	}

	this->clientSocket = clientSocket;
}
