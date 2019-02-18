#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "Socket.hpp"
#include "Address.hpp"
#include <cstring> // for memcpy

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

#include <assert.h>
#include <vector>
#include <map>
#include <stack>
#include <list>
#include <algorithm>
#include <functional>


namespace Net {

	class Connection
	{
	public:

		enum Mode
		{
			None,
			Client,
			Server
		};

		// Constructors
			Connection(unsigned int protocolId, float timeout);

		// Destructors
			virtual ~Connection();

		
		// Accessors
			bool ConnectFailed() const;
			bool IsRunning() const;
			bool IsConnecting() const;
			bool IsConnected() const;
			bool IsListening() const;
			int GetHeaderSize() const;
			Mode GetMode() const;

		// Functional
			bool Start(int port, std::string ip, bool client);
			void Connect(const Address & address);
			void Stop();
			void Listen();

			virtual void Update(float deltaTime);
			virtual bool SendPacket(const unsigned char data[], int size);
			virtual int ReceivePacket(unsigned char data[], int size);

	protected:

		// Functional
			virtual void OnStart() {}
			virtual void OnStop() {}
			virtual void OnConnect() {}
			virtual void OnDisconnect() {}

	private:

		// Functional
			void ClearData();

		enum State
		{
			Disconnected,
			Listening,
			Connecting,
			ConnectFail,
			Connected
		};


		// Data members
			unsigned int protocolId;
			float timeout;

			bool running;
			Mode mode;
			State state;
			Net::Socket socket;
			float timeoutAccumulator;
			Address address;
	};

}

#endif /* CONNECTION_HPP */