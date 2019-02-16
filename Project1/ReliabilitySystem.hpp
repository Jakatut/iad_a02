#ifndef RELIABILITYSYSTEM_HPP
#define RELIABILITYSYSTEM_HPP

#include "NetUtilities.hpp"
#include "PacketQueue.hpp"
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

	class ReliabilitySystem
	{
	public:

		// Functional
			ReliabilitySystem(unsigned int max_sequence = 0xFFFFFFFF);

			void Reset();

			void PacketSent(int size);

			void PacketReceived(unsigned int sequence, int size);

			unsigned int GenerateAckBits();

			void ProcessAck(unsigned int ack, unsigned int ack_bits);

			void Update(float deltaTime);

			void Validate();


		// Utility
			static bool sequence_more_recent(unsigned int s1, unsigned int s2, unsigned int max_sequence);

			static int bit_index_for_sequence(unsigned int sequence, unsigned int ack, unsigned int max_sequence);

			static unsigned int generate_ack_bits(unsigned int ack, const PacketQueue & received_queue, unsigned int max_sequence);

			static void process_ack(unsigned int ack, unsigned int ack_bits,
				PacketQueue & pending_ack_queue, PacketQueue & acked_queue,
				std::vector<unsigned int> & acks, unsigned int & acked_packets,
				float & rtt, unsigned int max_sequence);


		// Accessors
			unsigned int GetLocalSequence() const;

			unsigned int GetMaxSequence() const;

			void GetAcks(unsigned int** acks, int & count);

			unsigned int GetSentPackets() const;

			unsigned int GetReceivedPackets() const;

			unsigned int GetRemoteSequence() const;

			unsigned int GetLostPackets() const;

			unsigned int GetAckedPackets() const;

			float GetSentBandwidth() const;

			float GetAckedBandwidth() const;

			float GetRoundTripTime() const;

			int GetHeaderSize() const;

	protected:

		// Functional
			void AdvanceQueueTime(float deltaTime);
			void UpdateQueues();
			void UpdateStats();

	private:

		// Data members
			unsigned int max_sequence;			// maximum sequence value before wrap around (used to test sequence wrap at low # values)
			unsigned int local_sequence;		// local sequence number for most recently sent packet
			unsigned int remote_sequence;		// remote sequence number for most recently received packet

			unsigned int sent_packets;			// total number of packets sent
			unsigned int recv_packets;			// total number of packets received
			unsigned int lost_packets;			// total number of packets lost
			unsigned int acked_packets;			// total number of packets acked

			float sent_bandwidth;				// approximate sent bandwidth over the last second
			float acked_bandwidth;				// approximate acked bandwidth over the last second
			float rtt;							// estimated round trip time
			float rtt_maximum;					// maximum expected round trip time (hard coded to one second for the moment)

			std::vector<unsigned int> acks;		// acked packets from last set of packet receives. cleared each update!

			PacketQueue sentQueue;				// sent packets used to calculate sent bandwidth (kept until rtt_maximum)
			PacketQueue pendingAckQueue;		// sent packets which have not been acked yet (kept until rtt_maximum * 2 )
			PacketQueue receivedQueue;			// received packets for determining acks to send (kept up to most recent recv sequence - 32)
			PacketQueue ackedQueue;				// acked packets (kept until rtt_maximum * 2)
	};
}

#endif /* RELIABILITYSYSTEM_HPP */