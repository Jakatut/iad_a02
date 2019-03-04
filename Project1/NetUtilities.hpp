/*
	Simple Network Library from "Networking for Game Programmers"
	http://www.gaffer.org/networking-for-game-programmers
	Author: Glenn Fiedler <gaffer@gaffer.org>
*/

#ifndef NETUTILITIES_HPP
#define NETUTILITIES_HPP

// platform detection
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

#include "DataHash.hpp"

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

namespace Net
{

	const int HEADER_SIZE = 4;

	// platform independent wait for n seconds
	#if PLATFORM == PLATFORM_WINDOWS
		void wait(float seconds);
	#else
		#include <unistd.h>
		void wait(float seconds);
	#endif

	bool InitializeSockets();

	void ShutdownSockets();
	
	bool sequence_more_recent(unsigned int s1, unsigned int s2, unsigned int max_sequence);
	
	struct PacketData
	{
		unsigned int sequence;
		float time;
		int size;
	};

	enum class Mode {
	
		CLIENT,
		SERVER
	};

	struct Message {

		Message(int size);
		~Message();

		std::size_t Size();

		char Hash[MD5_OUTPUT_SIZE];
		char* Data;
	};

}

#endif /* NETUTILITIES_HPP */
