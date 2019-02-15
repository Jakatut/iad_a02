#ifndef FILETRANSFERCLIENT_HPP
#define FILETRANSFERCLIENT_HPP

#include "Address.hpp"
#include "UDPFileTransferConnection.hpp"
#include <exception>
#include <string>

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

	class UDPClient {

		private:
			Net::Address localAddress;
			SOCKET localSocket;

		public:

			/* CONSTRUCTOR WICH DOES WSA startup shit.s */
			UDPClient(Net::Address localAddress);

			Net::UDPConnection* ListenForConnection();

	};



	class ListenException : public std::runtime_error {

		public:
 			ListenException(const std::string& msg = "Listen Failed."): runtime_error(msg) {};	
	};

	class SocketSetupException : public std::runtime_error {

		public:
			SocketSetupException(const std::string& msg = "Socket setup failed."): runtime_error(msg) {};
	};
}

#endif /* FILETRANSFERCLIENT_HPP */