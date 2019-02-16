#ifndef RELIABLECONNECTION_HPP
#define RELIABLECONNECTION_HPP


#include "Connection.hpp"
#include "ReliabilitySystem.hpp"


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

	class ReliableConnection : public Net::Connection {

		public:

			// Constructors
				ReliableConnection(unsigned int protocolId, float timeout, unsigned int max_sequence = 0xFFFFFFFF): Connection(protocolId, timeout), reliabilitySystem(max_sequence)
				{
					ClearData();
					#ifdef NET_UNIT_TEST
						packet_loss_mask = 0;
					#endif
				}

			// Destructors
				~ReliableConnection();

			// Overriden Functions From "Connection"
				bool SendPacket(const unsigned char data[], int size);

				int ReceivePacket(unsigned char data[], int size);

				void Update(float deltaTime);

				int GetHeaderSize() const;

				ReliabilitySystem & GetReliabilitySystem();

			// Unit Test Controls
				#ifdef NET_UNIT_TEST
				void SetPacketLossMask(unsigned int mask);
				#endif

		protected:
			// Functional
				void WriteInteger(unsigned char * data, unsigned int value);

				void WriteHeader(unsigned char * header, unsigned int sequence, unsigned int ack, unsigned int ack_bits);

				void ReadInteger(const unsigned char * data, unsigned int & value);

				void ReadHeader(const unsigned char * header, unsigned int & sequence, unsigned int & ack, unsigned int & ack_bits);

				virtual void OnStop();

				virtual void OnDisconnect();

		private:
			// Functional
				void ClearData();

			// Data Members
				#ifdef NET_UNIT_TEST
					unsigned int packet_loss_mask;			// mask sequence number, if non-zero, drop packet - for unit test only
				#endif

				ReliabilitySystem reliabilitySystem;	// reliability system: manages sequence numbers and acks, tracks network stats etc.
	};
}

#endif /* RELIABLECONNECTION_HPP */