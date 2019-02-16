#include "ReliableConnection.hpp"

Net::ReliableConnection::~ReliableConnection() {

	if (IsRunning()) {
		Stop();
	}
}


bool Net::ReliableConnection::SendPacket(const unsigned char data[], int size) {

	#ifdef NET_UNIT_TEST
		if (reliabilitySystem.GetLocalSequence() & packet_loss_mask)
		{
			reliabilitySystem.PacketSent(size);
			return true;
		}
	#endif
	const int header = 12;
	unsigned char* packet = new unsigned char[header + size];
	unsigned int seq = reliabilitySystem.GetLocalSequence();
	unsigned int ack = reliabilitySystem.GetRemoteSequence();
	unsigned int ack_bits = reliabilitySystem.GenerateAckBits();
	WriteHeader(packet, seq, ack, ack_bits);
	std::memcpy(packet + header, data, size);
	if (!Connection::SendPacket(packet, size + header)) {

		delete packet;
		return false;
	}
	reliabilitySystem.PacketSent(size);
	delete packet;
	return true;
}


int Net::ReliableConnection::ReceivePacket(unsigned char data[], int size) {

	const int header = 12;
	if (size <= header) {

		return false;
	}
	unsigned char* packet = new unsigned char[header + size];
	int received_bytes = Connection::ReceivePacket(packet, size + header);
	if (received_bytes == 0) {

		delete packet;
		return false;
	}
	if (received_bytes <= header) {

		delete packet;
		return false;
	}
	unsigned int packet_sequence = 0;
	unsigned int packet_ack = 0;
	unsigned int packet_ack_bits = 0;
	ReadHeader(packet, packet_sequence, packet_ack, packet_ack_bits);
	reliabilitySystem.PacketReceived(packet_sequence, received_bytes - header);
	reliabilitySystem.ProcessAck(packet_ack, packet_ack_bits);
	std::memcpy(data, packet + header, received_bytes - header);
	delete packet;
	return received_bytes - header;
}


void Net::ReliableConnection::Update(float deltaTime) {

	Connection::Update(deltaTime);
	reliabilitySystem.Update(deltaTime);
}


int Net::ReliableConnection::GetHeaderSize() const {
	 
	return Connection::GetHeaderSize() + reliabilitySystem.GetHeaderSize();
}


Net::ReliabilitySystem& Net::ReliableConnection::GetReliabilitySystem() {

	return reliabilitySystem;
}


#ifdef NET_UNIT_TEST
	void Net::ReliableConnection::SetPacketLossMask(unsigned int mask) {

		packet_loss_mask = mask;
	}
#endif


// PROTECTED MEMBER METHODS

void Net::ReliableConnection::WriteInteger(unsigned char * data, unsigned int value) {

	data[0] = (unsigned char)(value >> 24);
	data[1] = (unsigned char)((value >> 16) & 0xFF);
	data[2] = (unsigned char)((value >> 8) & 0xFF);
	data[3] = (unsigned char)(value & 0xFF);
}


void Net::ReliableConnection::WriteHeader(unsigned char * header, unsigned int sequence, unsigned int ack, unsigned int ack_bits) {

	WriteInteger(header, sequence);
	WriteInteger(header + 4, ack);
	WriteInteger(header + 8, ack_bits);
}


void Net::ReliableConnection::ReadInteger(const unsigned char * data, unsigned int & value) {

	value = (((unsigned int)data[0] << 24) | ((unsigned int)data[1] << 16) |
		((unsigned int)data[2] << 8) | ((unsigned int)data[3]));
}


void Net::ReliableConnection::ReadHeader(const unsigned char * header, unsigned int & sequence, unsigned int & ack, unsigned int & ack_bits) {

	ReadInteger(header, sequence);
	ReadInteger(header + 4, ack);
	ReadInteger(header + 8, ack_bits);
}


void Net::ReliableConnection::OnStop() {

	ClearData();
}


void Net::ReliableConnection::OnDisconnect() {

	ClearData();
}


// PRIVATE MEMBER METHODS

void Net::ReliableConnection::ClearData() {

	reliabilitySystem.Reset();
}