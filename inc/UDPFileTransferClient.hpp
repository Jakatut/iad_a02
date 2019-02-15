#ifndef FILETRANSFERCLIENT_HPP
#define FILETRANSFERCLIENT_HPP

#include "Address.hpp"
#include "UDPFileTransferConnection.hpp"

#if defined(unix) || defined(__unix__) || defined(__unix)
	typedef int SOCKET;
#endif


namespace FileTransfer {

	class UDPClient {

		private:
			Net::Address address;
			SOCKET clientSocket;

		public:

			FileTransfer::UDPConnection* ListenForConnection();
	};
}



#endif /* FILETRANSFERCLIENT_HPP */