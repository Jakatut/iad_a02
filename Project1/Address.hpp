#ifndef ADDRESS_HPP
#define ADDRESS_HPP

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

	class Address {

		public:

			// Constructors
				Address();
				Address(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short port);
				Address(unsigned int address, unsigned short port);

			// Accessors
				unsigned int GetAddress() const;
				unsigned char GetA() const;
				unsigned char GetB() const;
				unsigned char GetC() const;
				unsigned char GetD() const;
				unsigned short GetPort() const;

			// Overloaded operators
				bool operator == (const Address & other) const;
				bool operator != (const Address & other) const;
				bool operator < (const Address & other) const;

		private:

			// Data members
				unsigned int address;
				unsigned short port;
	};
}

#endif /* ADDRESS_HPP */