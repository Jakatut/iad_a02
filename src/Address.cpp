#include "Address.hpp"
#include <string>
#include <iostream>

#if defined(unix) || defined(__unix__) || defined(__unix)
	#include <netinet/in.h>
	#include <arpa/inet.h>
#elif defined(_WIN32)
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#pragma comment(lib,"ws2_32.lib")
#endif

constexpr unsigned short minimumPort = 1024;


/*!
 *
 *	@brief <p>Address Constructor</p>
 *
 *	@detail <p>Validates that the port is not in the reserved values and that the ip address is valid.</p>
 *
 *	@param[in] <b>const std::string</b> - ipAddress - The ip address.
 *
 *	@param[in] <b>const unsigned short</b> - port - The port.
 *
 *	@throws invalid_argument - The port is invalid or the ip address is invalid.
 *
 */
Net::Address::Address(const std::string ipAddress, const unsigned short port): ipAddress(ipAddress), port(port) {

	if(port < minimumPort) {

		throw std::invalid_argument("Cannot use reserved ports: 0 - 1023.\n");		
	}

	struct sockaddr_in sockAddr;
	bool valid = false;

	#if defined(unix) || defined(__unix__) || defined(__unix) 
		valid = inet_pton(AF_INET, ipAddress.c_str(), &(sockAddr.sin_addr)) == 1;
	#elif defined(_WIN32)
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {

			std::cerr << "WASStartup failed.\n";
		}

		valid = InetPtonW(AF_INET, ipAddress.c_str(), &(sockAddr.sin_addr)) == 0;

		WSACleanup();
	#endif

	if(!valid) {

		throw std::invalid_argument("IP Address '" + ipAddress + "'is invalid.\n");
	}
}


/*!
 *
 *	@brief <p>Accessor for the ipAddress data member</p>
 *
 *	@return <b>std::string</b> - The ip address.
 *
 */
std::string Net::Address::GetIPAddress() {

	return ipAddress;
}


/*!
 *
 *	@brief <p>Mutator for the ipAddress data member.</p>
 *
 *	@details <p>Validates the ip before assigning it</p>
 * 
 *	@param[in] <b>const std::string</b> - ipAddress - The new ipAddress
 *
 *	@throws invalid_argument - The ip address is invalid.
 *
 */
void Net::Address::SetIPAddress(const std::string ipAddress) {


	struct sockaddr_in sockAddr;
	bool valid = false;

	#if defined(unix) || defined(__unix__) || defined(__unix) 
		valid = inet_pton(AF_INET, ipAddress.c_str(), &(sockAddr.sin_addr)) == 1;
	#elif defined(_WIN32)
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {

			std::cerr << "WASStartup failed.\n";
		}

		valid = InetPtonW(AF_INET, ipAddress.c_str(), &(sockAddr.sin_addr)) == 0;

		WSACleanup();
	#endif

	if(!valid) {

		throw std::invalid_argument("IP Address '" + ipAddress + "'is invalid.\n");
	}

	this->ipAddress = ipAddress;
}


/*!
 *
 *	@brief <p>Accessor for the port data member</p>
 *
 *	@return <b>unsigned short</b> - The port.
 *
 */
unsigned short Net::Address::GetPort() {

	return port;
}


/*!
 *
 *	@brief <p>Mutator for the port data member</p>
 *
 *	@details <p>Validates the port by checking if it is not in the reserved range (0 - 1023).</p>
 *
 *	@param[in] <b>unsigned short</b> - port - The new port.
 *
 *	@throws invalid_argument - the port is invalid.
 *
 */
void Net::Address::SetPort(const unsigned short port) {

	if(port < minimumPort) {

		throw std::invalid_argument("Cannot use reserved ports: 0 - 1023.\n");		
	}

	this->port = port;
}