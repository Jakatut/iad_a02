#include "ReliabilitySystem.hpp"


// PUBLIC MEMBER METHODS

Net::ReliabilitySystem::ReliabilitySystem(unsigned int max_sequence) {

	this->rtt_maximum = rtt_maximum;
	this->max_sequence = max_sequence;
	Reset();
}


void Net::ReliabilitySystem::Reset() {

	local_sequence = 0;
	remote_sequence = 0;
	sentQueue.clear();
	receivedQueue.clear();
	pendingAckQueue.clear();
	ackedQueue.clear();
	sent_packets = 0;
	recv_packets = 0;
	lost_packets = 0;
	acked_packets = 0;
	sent_bandwidth = 0.0f;
	acked_bandwidth = 0.0f;
	rtt = 0.0f;
	rtt_maximum = 1.0f;
}


void Net::ReliabilitySystem::PacketSent(int size) {

	if (sentQueue.exists(local_sequence))
	{
		printf("local sequence %d exists\n", local_sequence);
		for (PacketQueue::iterator itor = sentQueue.begin(); itor != sentQueue.end(); ++itor) {

			printf(" + %d\n", itor->sequence);
		}
	}
	assert(!sentQueue.exists(local_sequence));
	assert(!pendingAckQueue.exists(local_sequence));
	PacketData data;
	data.sequence = local_sequence;
	data.time = 0.0f;
	data.size = size;
	sentQueue.push_back(data);
	pendingAckQueue.push_back(data);
	sent_packets++;
	local_sequence++;
	if (local_sequence > max_sequence) {
		
		local_sequence = 0;
	}
}


void Net::ReliabilitySystem::PacketReceived(unsigned int sequence, int size) {

	recv_packets++;
	if (receivedQueue.exists(sequence)) {

		return;
	}
	PacketData data;
	data.sequence = sequence;
	data.time = 0.0f;
	data.size = size;
	receivedQueue.push_back(data);
	if (sequence_more_recent(sequence, remote_sequence, max_sequence)) {

		remote_sequence = sequence;
	}
}


unsigned int Net::ReliabilitySystem::GenerateAckBits() {

	return generate_ack_bits(GetRemoteSequence(), receivedQueue, max_sequence);
}


void Net::ReliabilitySystem::ProcessAck(unsigned int ack, unsigned int ack_bits) {
	 
	process_ack(ack, ack_bits, pendingAckQueue, ackedQueue, acks, acked_packets, rtt, max_sequence);
}


void Net::ReliabilitySystem::Update(float deltaTime) {

	acks.clear();
	AdvanceQueueTime(deltaTime);
	UpdateQueues();
	UpdateStats();
	#ifdef NET_UNIT_TEST
		Validate();
	#endif
}


void Net::ReliabilitySystem::Validate() {

	sentQueue.verify_sorted(max_sequence);
	receivedQueue.verify_sorted(max_sequence);
	pendingAckQueue.verify_sorted(max_sequence);
	ackedQueue.verify_sorted(max_sequence);
}


int Net::ReliabilitySystem::bit_index_for_sequence(unsigned int sequence, unsigned int ack, unsigned int max_sequence) {

	assert(sequence != ack);
	assert(!sequence_more_recent(sequence, ack, max_sequence));
	if (sequence > ack)
	{
		assert(ack < 33);
		assert(max_sequence >= sequence);
		return ack + (max_sequence - sequence);
	}
	else
	{
		assert(ack >= 1);
		assert(sequence <= ack - 1);
		return ack - 1 - sequence;
	}
}


bool Net::ReliabilitySystem::sequence_more_recent(unsigned int s1, unsigned int s2, unsigned int max_sequence)
{
	return (s1 > s2) && (s1 - s2 <= max_sequence / 2) || (s2 > s1) && (s2 - s1 > max_sequence / 2);
}


unsigned int Net::ReliabilitySystem::generate_ack_bits(unsigned int ack, const PacketQueue & received_queue, unsigned int max_sequence) {

	unsigned int ack_bits = 0;
	for (PacketQueue::const_iterator itor = received_queue.begin(); itor != received_queue.end(); itor++)
	{
		if (itor->sequence == ack || sequence_more_recent(itor->sequence, ack, max_sequence)) {

			break;
		}
		int bit_index = bit_index_for_sequence(itor->sequence, ack, max_sequence);
		if (bit_index <= 31) {

			ack_bits |= 1 << bit_index;
		}
	}
	return ack_bits;
}


void Net::ReliabilitySystem::process_ack(unsigned int ack, unsigned int ack_bits,
	PacketQueue & pending_ack_queue, PacketQueue & acked_queue,
	std::vector<unsigned int> & acks, unsigned int & acked_packets,
	float & rtt, unsigned int max_sequence) {

	if (pending_ack_queue.empty()) {

		return;
	}

	PacketQueue::iterator itor = pending_ack_queue.begin();
	while (itor != pending_ack_queue.end())
	{
		bool acked = false;

		if (itor->sequence == ack)
		{
			acked = true;
		}
		else if (!sequence_more_recent(itor->sequence, ack, max_sequence))
		{
			int bit_index = bit_index_for_sequence(itor->sequence, ack, max_sequence);
			if (bit_index <= 31) {

				acked = (ack_bits >> bit_index) & 1;
			}
		}

		if (acked)
		{
			rtt += (itor->time - rtt) * 0.1f;

			acked_queue.insert_sorted(*itor, max_sequence);
			acks.push_back(itor->sequence);
			acked_packets++;
			itor = pending_ack_queue.erase(itor);
		}
		else {
			++itor;
		}
	}
}


unsigned int Net::ReliabilitySystem::GetLocalSequence() const {

	return local_sequence;
}


unsigned int Net::ReliabilitySystem::GetMaxSequence() const {

	return max_sequence;
}


void Net::ReliabilitySystem::GetAcks(unsigned int ** acks, int & count) {

	if (!this->acks.empty()) {

		*acks = &this->acks[0];
		count = (int)this->acks.size();
	}
}


unsigned int Net::ReliabilitySystem::GetSentPackets() const {

	return sent_packets;
}


unsigned int Net::ReliabilitySystem::GetReceivedPackets() const {

	return recv_packets;
}


unsigned int Net::ReliabilitySystem::GetRemoteSequence() const
{
	return remote_sequence;
}


unsigned int Net::ReliabilitySystem::GetLostPackets() const {

	return lost_packets;
}


unsigned int Net::ReliabilitySystem::GetAckedPackets() const {

	return acked_packets;
}


float Net::ReliabilitySystem::GetSentBandwidth() const {

	return sent_bandwidth;
}


float Net::ReliabilitySystem::GetAckedBandwidth() const {

	return acked_bandwidth;
}


float Net::ReliabilitySystem::GetRoundTripTime() const {

	return rtt;
}


int Net::ReliabilitySystem::GetHeaderSize() const {

	return 12;
}



// PROTECTED MEMBER METHODS

void Net::ReliabilitySystem::AdvanceQueueTime(float deltaTime) {

	for (PacketQueue::iterator itor = sentQueue.begin(); itor != sentQueue.end(); itor++) {

		itor->time += deltaTime;
	}

	for (PacketQueue::iterator itor = receivedQueue.begin(); itor != receivedQueue.end(); itor++) {

		itor->time += deltaTime;
	}

	for (PacketQueue::iterator itor = pendingAckQueue.begin(); itor != pendingAckQueue.end(); itor++) {

		itor->time += deltaTime;
	}

	for (PacketQueue::iterator itor = ackedQueue.begin(); itor != ackedQueue.end(); itor++) {

		itor->time += deltaTime;
	}
}


void Net::ReliabilitySystem::UpdateQueues() {

	const float epsilon = 0.001f;

	while (sentQueue.size() && sentQueue.front().time > rtt_maximum + epsilon) {

		sentQueue.pop_front();
	}

	if (receivedQueue.size())
	{
		const unsigned int latest_sequence = receivedQueue.back().sequence;
		const unsigned int minimum_sequence = latest_sequence >= 34 ? (latest_sequence - 34) : max_sequence - (34 - latest_sequence);
		while (receivedQueue.size() && !sequence_more_recent(receivedQueue.front().sequence, minimum_sequence, max_sequence))
			receivedQueue.pop_front();
	}

	while (ackedQueue.size() && ackedQueue.front().time > rtt_maximum * 2 - epsilon) {

		ackedQueue.pop_front();
	}

	while (pendingAckQueue.size() && pendingAckQueue.front().time > rtt_maximum + epsilon)
	{
		pendingAckQueue.pop_front();
		lost_packets++;
	}
}


void Net::ReliabilitySystem::UpdateStats() {

	int sent_bytes_per_second = 0;
	for (PacketQueue::iterator itor = sentQueue.begin(); itor != sentQueue.end(); ++itor) {

		sent_bytes_per_second += itor->size;
	}

	int acked_packets_per_second = 0;
	int acked_bytes_per_second = 0;
	for (PacketQueue::iterator itor = ackedQueue.begin(); itor != ackedQueue.end(); ++itor)
	{
		if (itor->time >= rtt_maximum)
		{
			acked_packets_per_second++;
			acked_bytes_per_second += itor->size;
		}
	}
	sent_bytes_per_second /= rtt_maximum;
	acked_bytes_per_second /= rtt_maximum;
	sent_bandwidth = sent_bytes_per_second * (8 / 1000.0f);
	acked_bandwidth = acked_bytes_per_second * (8 / 1000.0f);
}