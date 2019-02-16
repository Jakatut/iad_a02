#include "NetUtilities.hpp"


#if PLATFORM == PLATFORM_WINDOWS
	void Net::wait(float seconds)
	{
		Sleep((int)(seconds * 1000.0f));
	}
#else
	#include <unistd.h>
	void Net::wait(float seconds) {
		usleep((int)(seconds * 1000000.0f)); 
	}
#endif

bool Net::InitializeSockets()
{
	#if PLATFORM == PLATFORM_WINDOWS
		WSADATA WsaData;
		return WSAStartup(MAKEWORD(2, 2), &WsaData) == NO_ERROR;
	#else
		return true;
	#endif
}

void Net::ShutdownSockets() {

	#if PLATFORM == PLATFORM_WINDOWS
		WSACleanup();
	#endif
}

bool Net::sequence_more_recent(unsigned int s1, unsigned int s2, unsigned int max_sequence) {

	auto half_max = max_sequence / 2;
	return (((s1 > s2) && (s1 - s2 <= half_max))
			||
			((s2 > s1) && (s2 - s1 > half_max)));
}


