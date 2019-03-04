#include "CommandLineParser.hpp"

CommandLineParser::CommandLineParser(std::map<std::string, bool> switches, std::string delimiter) {

	this->switches = switches;
	this->delimiter = delimiter;
}

std::map<std::string, std::string> CommandLineParser::GetArguments(int argc, char* argv[]) {

	std::map<std::string, std::string> args;

	for (int i = 1; i < argc; ++i) {

		auto itr = switches.begin();
		if ((itr = switches.find(argv[i])) != switches.end()) {

			if (itr->second) {
				if (delimiter == " ") {

					args.insert(std::make_pair(itr->first, argv[i + 1]));
					++i;
				}
				else {

					std::string argument{ argv[i] };

					args.insert(std::make_pair(itr->first, argument.substr(argument.find(delimiter), argument.size())));
				}
			}
			else {

				args.insert(std::make_pair(itr->first, ""));
			}
		}
	}

	for (auto itr = switches.begin(); itr != switches.end(); ++itr) {

		if (args.find(itr->first) == args.end()) {

			args.insert(std::make_pair(itr->first, ""));
		}
	}

	return args;
}


void CommandLineParser::addSwitch(std::pair<std::string, bool> newSwitch) {

	this->switches.insert(newSwitch);
}


void CommandLineParser::removeSwitch(std::string removeSwitch) {

	switches.erase(removeSwitch);
}
