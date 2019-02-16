#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "Address.hpp"

#define PLATFORM_WINDOWS  1
#define PLATFORM_MAC      2
#define PLATFORM_UNIX     3

#if defined(_WIN32)
#define PLATFORM PLATFORM_WINDOWS
#elif defined(__APPLE__)
	#define PLATFORM PLATFORM_MAC
#else
	#define PLATFORM PLATFORM_UNIX
#endif
#if PLATFORM == PLATFORM_WINDOWS
	#include <winsock2.h>
	#pragma comment( lib, "wsock32.lib" )
#elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <fcntl.h>
#else
	#error unknown platform!
#endif

#include <cstring>
#include <assert.h>
#include <vector>
#include <map>
#include <stack>
#include <list>
#include <algorithm>
#include <functional>

namespace Net {

	class Socket {

		public:

			// Constructors 
				Socket();

			// Destructors
				~Socket();

			// Functional
				void Close();
				bool Open(unsigned short port);
				bool IsOpen() const;
				bool Send(const Address & destination, const void * data, int size);
				int Receive(Address & sender, void * data, int size);

		private:

			// Data members
				int socket;
	};

}


#endif /* SOCKET_HPP */