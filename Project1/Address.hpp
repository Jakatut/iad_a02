#include <string>

namespace Net {

	class Address {

		private:
			std::string ipAddress;
			unsigned short port;

		public:

			Address(const std::string ipAddress, const unsigned short port);
			
			std::string GetIPAddress();
			void SetIPAddress(const std::string ipAddress);

			unsigned short GetPort();
			void ChangePort(const unsigned short port);
	};
}