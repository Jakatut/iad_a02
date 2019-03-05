#ifndef DATAHASH_HPP
#define DATAHASH_HPP

#include <openssl\md5.h>
#include <string>
#include <sstream>
#include <vector>

constexpr int MD5_OUTPUT_SIZE = 33;
constexpr int MD5_HALF_HASH_SIZE = 16;

class DataHash
{
	public:

		std::string static MD5HashData(const char* dataString);
		std::string static MD5HashData(const std::string& dataString);
		std::string static MD5HashData(const unsigned char* dataString);
		std::stringstream static MD5HashData(const std::stringstream& dataSStream);
		std::vector<unsigned char> static MD5HashData(const std::vector<unsigned char>& dataVector);
};


#endif /* DATAHASH_HPP */