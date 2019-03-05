#include "DataHash.hpp"

#pragma warning (disable:4996)


std::string DataHash::MD5HashData(const char* dataString) {

	unsigned char digest[MD5_DIGEST_LENGTH];

	MD5((unsigned char*)dataString, strlen(dataString), digest);

	char mdString[MD5_OUTPUT_SIZE];

	for (int i = 0; i < MD5_HALF_HASH_SIZE; i++) {

		sprintf(&mdString[i * 2], "%02x", digest[i]);
	}

	return std::string{ mdString };
}


std::string DataHash::MD5HashData(const std::string& dataString) {

	if (dataString.empty()) {

		return dataString;
	}

	unsigned char digest[MD5_DIGEST_LENGTH] = {0};

	MD5((unsigned char *)dataString.c_str(), dataString.size(), digest);

	char mdString[MD5_OUTPUT_SIZE];

	for (int i = 0; i < MD5_HALF_HASH_SIZE; i++) {
	
		sprintf(&mdString[i * 2], "%02x", digest[i]);
	}

	return std::string{mdString};
}

std::string DataHash::MD5HashData(const unsigned char * dataString)
{
	unsigned char digest[MD5_DIGEST_LENGTH];

	MD5((unsigned char*)dataString, strlen(reinterpret_cast<const char*>(dataString)), digest);

	char mdString[MD5_OUTPUT_SIZE];

	for (int i = 0; i < MD5_HALF_HASH_SIZE; i++) {

		sprintf(&mdString[i * 2], "%02x", digest[i]);
	}

	return std::string{ mdString };
}


std::stringstream DataHash::MD5HashData(const std::stringstream& dataSStream) {

	if (dataSStream.str().empty()) {

		return std::stringstream{};
	}

	unsigned char digest[MD5_DIGEST_LENGTH] = { 0 };

	MD5((unsigned char *)dataSStream.str().c_str(), dataSStream.str().size(), digest);

	char mdString[MD5_OUTPUT_SIZE];

	for (int i = 0; i < MD5_HALF_HASH_SIZE; i++) {

		sprintf(&mdString[i * 2], "%02x", digest[i]);
	}

	return std::stringstream{ mdString };
}



std::vector<unsigned char> DataHash::MD5HashData(const std::vector<unsigned char>& dataVector) {

	if (dataVector.empty()) {

		return dataVector;
	}

	unsigned char MD5digest[MD5_DIGEST_LENGTH] = { 0 };

	MD5((unsigned char *)&dataVector[0], dataVector.size(), MD5digest);

	char mdString[MD5_OUTPUT_SIZE];

	for (int i = 0; i < MD5_HALF_HASH_SIZE; i++) {

		sprintf(&mdString[i * 2], "%02x", MD5digest[i]);
	}

	std::vector<unsigned char> hashVector(mdString, mdString + MD5_OUTPUT_SIZE - 1);

	return hashVector;
}
