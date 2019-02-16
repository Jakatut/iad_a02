#ifndef COMMANDLINEPARSER_HPP
#define COMMANDLINEPARSER_HPP


#include <map>
#include <vector>
#include <string>


class CommandLineParser
{
	std::map<std::string, bool> switches;
	std::string delimiter;

	public:
		CommandLineParser(std::map<std::string, bool> switches, std::string delimiter);
		
		std::map<std::string, std::string> GetArguments(int argc, char* argv[]);

		void addSwitch(std::pair<std::string, bool> newSwitch);
		void removeSwitch(std::string removeSwitch);
};

#endif /* COMMANDLINEPARSER_HPP */
 
