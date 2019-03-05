#include "DataHash.hpp"
#include "NetUtilities.hpp"
#include "Connection.hpp"
#include <iostream>

constexpr int MINIMUM_PORT = 1024;

// PUBLIC MEMBER METHODS

Net::Connection::Connection(unsigned int protocolId, float timeout) {

	this->protocolId = protocolId;
	this->timeout = timeout;
	mode = None;
	running = false;
	ClearData();
}


Net::Connection::~Connection() {

	if (IsRunning())
		Stop();
}


bool Net::Connection::Start(int port, std::string ip, bool client) {

	if (port < MINIMUM_PORT) {

		std::cout << "Cannot use reserved ports: 0 - 1023.\n";
		return false;
	}

	assert(!running);
	std::cout << "start connection on port " << port << '\n';
	if (!socket.Open(port, ip, client)) {

		return false;
	}
	running = true;
	OnStart();
	return true;
}


void Net::Connection::Stop() {

	assert(running);
	std::cout << "stop connection\n";
	bool connected = IsConnected();
	ClearData();
	socket.Close();
	running = false;
	if (connected) {

		OnDisconnect();
	}
	OnStop();
}


bool Net::Connection::IsRunning() const {

	return running;
}


void Net::Connection::Listen() {

	std::cout << "server listening for connection\n";
	bool connected = IsConnected();
	ClearData();
	if (connected) {

		OnDisconnect();
	}
	mode = Server;
	state = Listening;
}


void Net::Connection::Connect(const Address & address) {

	std::cout << "client connecting to " << static_cast<unsigned short>(address.GetA()) << '.' 
											<< static_cast<unsigned short>(address.GetB()) << '.' 
											<< static_cast<unsigned short>(address.GetC()) << '.'
											<< static_cast<unsigned short>(address.GetD())
											<< ": " << address.GetPort() << '\n';
	bool connected = IsConnected();
	ClearData();
	if (connected) {

		OnDisconnect();
	}
	mode = Client;
	state = Connecting;
	this->address = address;
}


bool Net::Connection::IsConnecting() const {

	return state == Connecting;
}


bool Net::Connection::ConnectFailed() const {

	return state == ConnectFail;
}


bool Net::Connection::IsConnected() const {

	return state == Connected;
}


bool Net::Connection::IsListening() const {

	return state == Listening;
}


Net::Connection::Mode Net::Connection::GetMode() const {

	return mode;
}


void Net::Connection::Update(float deltaTime) {

	assert(running);
	timeoutAccumulator += deltaTime;
	if (timeoutAccumulator > timeout)
	{
		if (state == Connecting)
		{
			std::cout << "connect timed out\n";
			ClearData();
			state = ConnectFail;
			OnDisconnect();
		}
		else if (state == Connected)
		{
			std::cout << "connection timed out\n";
			ClearData();
			if (state == Connecting) {

				state = ConnectFail;
			}
			OnDisconnect();
		}
	}
}


bool Net::Connection::SendPacket(const unsigned char data[], int size) {

	assert(running);
	if (address.GetAddress() == 0) {

		return false;
	}

	unsigned char* packet = new unsigned char[size + 4];
	packet[0] = (unsigned char)(protocolId >> 24);
	packet[1] = (unsigned char)((protocolId >> 16) & 0xFF);
	packet[2] = (unsigned char)((protocolId >> 8) & 0xFF);
	packet[3] = (unsigned char)((protocolId) & 0xFF);
	std::memcpy(&packet[4], data, size);

	Net::Message message(size + 4);
	std::memcpy(message.Data, packet, size + 4);
	std::memcpy(message.Checksum, DataHash::MD5HashData(packet).c_str(), MD5_OUTPUT_SIZE);

	bool send = socket.Send(address, &message, size + 4);

	delete packet;
	return send;
}


int Net::Connection::ReceivePacket(unsigned char data[], int size) {

	assert(running);

	Net::Message message(size + 4);

	Address sender;
	int bytes_read = socket.Receive(sender, &message, size + 4);

	// If the data hash is not the same as the one recieved, exit.
	if (DataHash::MD5HashData(message.Data) != std::string{ message.Checksum }) {

		return 0;
	}

	if (bytes_read == 0) {

		return 0;
	}
	if (bytes_read <= 4) {

		return 0;
	}
	if (message.Data[0] != (unsigned char)(protocolId >> 24) ||
		message.Data[1] != (unsigned char)((protocolId >> 16) & 0xFF) ||
		message.Data[2] != (unsigned char)((protocolId >> 8) & 0xFF) ||
		message.Data[3] != (unsigned char)(protocolId & 0xFF)) {

		return 0;
	}
	if (mode == Server && !IsConnected())
	{
		std::cout << "server accepts connection from client " << sender.GetA() << sender.GetB() << sender.GetC() << sender.GetD() << sender.GetPort() << '\n';
		state = Connected;
		address = sender;
		OnConnect();
	}
	if (sender == address)
	{
		if (mode == Client && state == Connecting)
		{
			std::cout << "client completes connection with server\n";
			state = Connected;
			OnConnect();
		}
		timeoutAccumulator = 0.0f;
		memcpy(data, &message.Data[4], bytes_read - 4);

		return bytes_read - 4;
	}

	return 0;
}


int Net::Connection::GetHeaderSize() const {

	return 4;
}



// PRIVATE MEMBER METHODS

void Net::Connection::ClearData()
{
	state = Disconnected;
	timeoutAccumulator = 0.0f;
	address = Address();
}