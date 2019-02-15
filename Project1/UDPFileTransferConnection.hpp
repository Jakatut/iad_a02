#ifndef FILETRANSFERCONNECTION_HPP
#define FILETRANSFERCONNECTION_HPP

#include "Address.hpp"
#include <sstream>
#include <string>
#include <vector>

#if defined(unix) || defined(__unix__) || defined(__unix)
	#include <netinet/in.h>
	#include <arpa/inet.h>
	typedef int SOCKET;
#elif defined(_WIN32)
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#pragma comment(lib,"ws2_32.lib")
#endif


namespace Net {

	class UDPConnection {

		private:

			Net::Address connectedAddress;
			SOCKET clientSocket;
		public:
			
			UDPConnection(SOCKET clientSocket, Net::Address connectedAddress);


			int SendData(std::vector<unsigned char>& data);
			std::vector<unsigned char> ReceiveData();
			void closeConnection();
	};


	class ConnectionException : public std::runtime_error {

	public:
		ConnectionException(const std::string& msg = "Invalid connection.") : runtime_error(msg) {};
	};
}


#endif /* FILETRANSFERCONNECTION_HPP */