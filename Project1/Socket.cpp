#include "Socket.hpp"

#pragma warning (disable:4996)

Net::Socket::Socket()
{
	socket = 0;
}


Net::Socket::~Socket()
{
	Close();
}


bool Net::Socket::Open(unsigned short port, std::string ip, bool client)
{
	assert(!IsOpen());

	// create socket

	socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (socket == INVALID_SOCKET)
	{
		printf("failed to create socket\n");
		socket = 0;
		return false;
	}

	// bind to port
	sockaddr_in address;
	address.sin_family = AF_INET;

	if (client) {
		
		address.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
	}
	else {

		address.sin_addr.s_addr = INADDR_ANY;
	}
	address.sin_port = htons(port);

	if (!client && ::bind(socket, (const sockaddr*)&address, sizeof(address)) == SOCKET_ERROR)
	{
		printf("failed to bind socket\n");
		Close();
		return false;
	}

	// set non-blocking io
	#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX

		int nonBlocking = 1;
		if (fcntl(socket, F_SETFL, O_NONBLOCK, nonBlocking) == -1)
		{
			printf("failed to set non-blocking socket\n");
			Close();
			return false;
		}

	#elif PLATFORM == PLATFORM_WINDOWS

		DWORD nonBlocking = 1;
		if (ioctlsocket(socket, FIONBIO, &nonBlocking) != 0)
		{
			printf("failed to set non-blocking socket\n");
			Close();
			return false;
		}

	#endif

	return true;
}


void Net::Socket::Close()
{
	if (socket != 0)
	{
		#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
			close(socket);
		#elif PLATFORM == PLATFORM_WINDOWS
			closesocket(socket);
		#endif

		socket = 0;
	}
}


bool Net::Socket::IsOpen() const
{
	return socket != 0;
}


bool Net::Socket::Send(const Net::Address & destination, const void* data, int size)
{
	assert(data);
	assert(size > 0);

	if (socket == 0) {

		return false;
	}

	assert(destination.GetAddress() != 0);
	assert(destination.GetPort() != 0);

	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(destination.GetAddress());
	address.sin_port = htons((unsigned short)destination.GetPort());

	int sent_bytes = sendto(socket, reinterpret_cast<const char*>(data), size, 0, (sockaddr*)&address, sizeof(address));

	return sent_bytes == size;
}


int Net::Socket::Receive(Net::Address & sender, void* data, int size)
{
	assert(data);
	assert(size > 0);

	if (socket == 0) {

		return false;
	}

	#if PLATFORM == PLATFORM_WINDOWS
		typedef int socklen_t;
	#endif

	sockaddr_in from;
	socklen_t fromLength = sizeof(from);

	int received_bytes = recvfrom(socket, reinterpret_cast<char*>(data), size, 0, (sockaddr*)&from, &fromLength);

	if (received_bytes <= 0) {

		return 0;
	}

	unsigned int address = ntohl(from.sin_addr.s_addr);
	unsigned short port = ntohs(from.sin_port);

	sender = Net::Address(address, port);

	return received_bytes;
}
