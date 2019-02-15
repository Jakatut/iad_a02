#ifndef FILETRANSFERCONNECTION_HPP
#define FILETRANSFERCONNECTION_HPP

#include "Address.hpp"
#include <sstream>
#include <string>
#include <vector>

#if defined(unix) || defined(__unix__) || defined(__unix)
	typedef int SOCKET;
#endif


namespace FileTransfer {

	class UDPConnection {

		private:
			Net::Address connectedAddress;
			SOCKET connectionSocket;

		public:
			UDPConnection(SOCKET connectionSocket, Net::Address connectedAddress): 
				connectedAddress(connectedAddress),connectionSocket(connectionSocket) {};

			int SendData(std::vector<unsigned char>& data);
			std::vector<unsigned char> ReceiveData();
			void closeConnection();
	};
}


#endif /* FILETRANSFERCONNECTION_HPP */