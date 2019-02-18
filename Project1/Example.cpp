/*
	Reliability and Flow Control Example
	From "Networking for Game Programmers" - http://www.gaffer.org/networking-for-game-programmers
	Author: Glenn Fiedler <gaffer@gaffer.org>
*/

#include "FileIO.hpp"
#include "CommandLineParser.hpp"
#include "NetUtilities.hpp"
#include "FlowControl.hpp"
#include "Socket.hpp"
#include "Address.hpp"
#include "Connection.hpp"
#include "PacketQueue.hpp"
#include "ReliabilitySystem.hpp"
#include "ReliableConnection.hpp"
#include "DataHash.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>



//#define SHOW_ACKS
#pragma warning (disable:4996)


const int ServerPort = 30000;
const int ClientPort = 30001;
const int ProtocolId = 0x11223344;
const float DeltaTime = 1.0f / 30.0f;
const float SendRate = 1.0f / 30.0f;
const float TimeOut = 10.0f;
const int PacketSize = 256;
constexpr int CLIENT_ARGUMENT_COUNT = 6;

int main( int argc, char * argv[])
{

	unsigned short port = 0;

	// Parse command line arguments.
	std::map<std::string, bool> switches{
	
		{"-ip", true},
		{"-port", true},
		{"-file", true}
	};
	CommandLineParser parser{switches, " "};
	std::map<std::string, std::string> arguments = parser.GetArguments(argc, argv);


	// Get the file data.
	std::string fileData = "No file selected.";
	
	if (arguments.find("-file") != arguments.end()) {

		FileIO input{  arguments.at("-file") };
		input.Read();
		fileData = input.GetTextRead();
	}


	Net::Address address;
	Net::Mode mode;

	// Determine if the mode is client or server.
	if (argc >= CLIENT_ARGUMENT_COUNT) {

		mode = Net::Mode::CLIENT;

		// Get the ip and port for the client connection address.
		if ((arguments.find("-ip") != arguments.end()) && (arguments.find("-ip") != arguments.end())) {

			int ipA = 0;
			int ipB = 0;
			int ipC = 0;
			int ipD = 0;
			if (sscanf(arguments.at("-ip").c_str(), "%d.%d.%d.%d", &ipA, &ipB, &ipC, &ipD)) {

				address = Net::Address(ipA, ipB, ipC, ipD, static_cast<unsigned short>(std::stoi(arguments.at("-port"))));

				port = static_cast<unsigned short>(std::stoi(arguments.at("-port")));
			}
		}
	}
	else {

		mode = Net::Mode::SERVER;

		// Get port for server to listen.
		if (arguments.find("-port") != arguments.end()) {

			port = static_cast<unsigned short>(std::stoi(arguments.at("-port")));
		}
	}

	// WSA startup
	if (!Net::InitializeSockets()) {

		std::cout << "Failed to initalize sockets.\n";
		return 1;
	}

	Net::ReliableConnection connection(ProtocolId, TimeOut);


	if (mode == Net::Mode::CLIENT) {

		if (!connection.Start(port, arguments.at("-ip"), mode == Net::Mode::CLIENT)) {

			std::cout << "Unable to start connection on port " << port << '\n';
			Net::ShutdownSockets();
			return 1;
		}

		connection.Connect(address);
	}
	else {

		if (!connection.Start(port, "", mode == Net::Mode::CLIENT)) {

			std::cout << "Unable to start connection on port " << port << '\n';
			Net::ShutdownSockets();
			return 1;
		}

		connection.Listen();
	}

	
	bool connected = false;
	float sendAccumulator = 0.0f;
	float statsAccumulator = 0.0f;
	
	FlowControl flowControl;
	
	while (true)
	{
		// update flow control
		if (connection.IsConnected()) {

			flowControl.Update(DeltaTime, connection.GetReliabilitySystem().GetRoundTripTime() * 1000.0f);
		}
		
		const float sendRate = flowControl.GetSendRate();

		// detect changes in connection state
		if (mode == Net::Mode::SERVER && connected && !connection.IsConnected())
		{
			flowControl.Reset();
			printf( "reset flow control\n" );
			connected = false;
		}

		if ( !connected && connection.IsConnected() )
		{
			printf( "client connected to server\n" );
			connected = true;
		}
		
		if ( !connected && connection.ConnectFailed() )
		{
			printf( "connection failed\n" );
			break;
		}
		
		// send and receive packets
		sendAccumulator += DeltaTime;
		
		while ( sendAccumulator > 1.0f / sendRate )
		{
			//unsigned char packet[PacketSize] = "Hello, World!";
			//connection.SendPacket(packet, sizeof(packet));
			connection.SendPacket( reinterpret_cast<const unsigned char*>(fileData.c_str()), fileData.size() + 1 );
			sendAccumulator -= 1.0f / sendRate;
		}
		
		

		while ( mode == Net::Mode::SERVER )
		{
			unsigned char packet[256] = {0};
			int bytes_read = connection.ReceivePacket( packet, sizeof(packet) );
			
			if (bytes_read == 0) {
				
				break;
			}
			
			FileIO output("received.3gp", true, true);
			output.Write(reinterpret_cast<const char*>(packet));

		}

		
		// show packets that were acked this frame
		#ifdef SHOW_ACKS
			unsigned int * acks = NULL;
			int ack_count = 0;
			connection.GetReliabilitySystem().GetAcks( &acks, ack_count );
			if ( ack_count > 0 )
			{
				printf( "acks: %d", acks[0] );
				for ( int i = 1; i < ack_count; ++i )
					printf( ",%d", acks[i] );
				printf( "\n" );
			}
		#endif

		// update connection
		connection.Update( DeltaTime );

		// show connection stats
		statsAccumulator += DeltaTime;

		while ( statsAccumulator >= 0.25f && connection.IsConnected() )
		{
			float rtt = connection.GetReliabilitySystem().GetRoundTripTime();
			
			unsigned int sent_packets = connection.GetReliabilitySystem().GetSentPackets();
			unsigned int acked_packets = connection.GetReliabilitySystem().GetAckedPackets();
			unsigned int lost_packets = connection.GetReliabilitySystem().GetLostPackets();
			
			float sent_bandwidth = connection.GetReliabilitySystem().GetSentBandwidth();
			float acked_bandwidth = connection.GetReliabilitySystem().GetAckedBandwidth();
			
			/*printf( "rtt %.1fms, sent %d, acked %d, lost %d (%.1f%%), sent bandwidth = %.1fkbps, acked bandwidth = %.1fkbps\n", 
				rtt * 1000.0f, sent_packets, acked_packets, lost_packets, 
				sent_packets > 0.0f ? (float) lost_packets / (float) sent_packets * 100.0f : 0.0f, 
				sent_bandwidth, acked_bandwidth);*/
			
			statsAccumulator -= 0.25f;
		}

		Net::wait( DeltaTime );
	}
	
	Net::ShutdownSockets();
	Net::ShutdownSockets();

	return 0;
}
