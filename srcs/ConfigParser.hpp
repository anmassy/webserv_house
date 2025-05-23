#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <sstream>
#include <vector>

//recupere le pass et les istruction donnee
struct Location {
	std::string path;
	std::map<std::string, std::string> instruct;
};

// recupere les instruction donnee et gere LES block location grace a vector qui les prend en consideration
struct Server {
	std::vector<Location> locations;
	std::map<std::string, std::string> instruct;
};

class ConfigParser {
	private :
		int		_blockDepth;
		bool	_inServer;
		bool	_inLocation;

		std::vector<Server>	_servers;
		Server				_currentServer;
		Location			_currentLocation;

		bool	openFile(const std::string& filename, std::ifstream& file);
		bool	parseLine(const std::string& line);
		bool	handleServerBlock();
		bool	handleLocationBlock(std::istringstream& iss);
		bool	handleClosingBrace();
		void	assignKeyValue(const std::string& key, std::istringstream& iss);
		
		std::string	trim(const std::string& s);

	public :
		ConfigParser();
		~ConfigParser();

		bool parseFile(const std::string& filename);
		void printServers() const;
		const std::vector<Server>&	getServers() const;

};



#endif