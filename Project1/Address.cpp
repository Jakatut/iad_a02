#include "Address.hpp"

Net::Address::Address() {

	address = 0;
	port = 0;
}


Net::Address::Address(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short port) {

	this->address = (a << 24) | (b << 16) | (c << 8) | d;
	this->port = port;
}

Net::Address::Address(unsigned int address, unsigned short port) {


	this->address = address;
	this->port = port;
}


unsigned int Net::Address::GetAddress() const {

	return address;
}


unsigned char Net::Address::GetA() const {

	return (unsigned char)(address >> 24);
}

unsigned char Net::Address::GetB() const {

	return (unsigned char)(address >> 16);
}

unsigned char Net::Address::GetC() const {

	return (unsigned char)(address >> 8);
}

unsigned char Net::Address::GetD() const {

	return (unsigned char)(address);
}


unsigned short Net::Address::GetPort() const {

	return port;
}


bool Net::Address::operator== (const Address & other) const {

	return address == other.address && port == other.port;
}

bool Net::Address::operator!= (const Address & other) const {

	return !(*this == other);
}

bool Net::Address::operator< (const Address & other) const {

	// note: this is so we can use address as a key in std::map
	if (address < other.address)
		return true;
	if (address > other.address)
		return false;
	else
		return port < other.port;
}