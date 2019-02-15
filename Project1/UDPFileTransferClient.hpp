#pragma once
#include "Address.hpp"
#include "UDPFileTransferConnection.hpp"

class UDPFileTransferClient
{

	private:
		Net::Address localAddress;
		SOCKET localSocket;

	public:
		UDPFileTransferClient();

		bool ConnectToServer(Net::Address serverAddress);
		bool SendData(std::vector<unsigned char> data);
};

