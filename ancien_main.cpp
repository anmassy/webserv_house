#include "checkConf.hpp"

// Trim début/fin
std::string trim(const std::string& s) {
	size_t start = s.find_first_not_of(" \t\r\n");
	size_t end = s.find_last_not_of(" \t\r\n");
	if (start == std::string::npos || end == std::string::npos)
		return "";
	return s.substr(start, end - start + 1);
}

// Ouvre et vérifie le fichier
bool openConfigFile(const char* path, std::ifstream& file) {
	file.open(path);
	if (!file.is_open()) {
		std::cerr << "Erreur : impossible d'ouvrir le fichier." << std::endl;
		return false;
	}
	return true;
}

// Parse le fichier
bool parseConfig(std::ifstream& file, std::vector<Server>& servers) {
	Server server;
	Location location;
	bool inServer = false;
	bool inLocation = false;
	int blockDepth = 0;
	std::string line;

	while (std::getline(file, line)) {
		// Supprimer commentaires inline
		size_t commentPos = line.find('#');
		if (commentPos != std::string::npos)
			line = line.substr(0, commentPos);
		line = trim(line);
		if (line.empty())
			continue;

		if (line.back() == ';')
			line.erase(line.size() - 1);

		//separer cette parti en 3 blocks
		std::istringstream iss(line);
		std::string title;
		iss >> title;

		if (title == "server") {
			if (blockDepth != 0) {
				std::cerr << "Erreur : bloc 'server' mal placé." << std::endl;
				return false;
			}
			inServer = true;
			server = Server();
			blockDepth = 1;
			continue;
		}
		else if (title == "location") {
			if (blockDepth != 1) {
				std::cerr << "Erreur : bloc 'location' en dehors d'un 'server'." << std::endl;
				return false;
			}
			std::string path;
			iss >> path;
			location = Location();
			location.path = path;
			inLocation = true;
			blockDepth = 2;
			continue;
		}
		else if (title == "}") {
			if (blockDepth == 2) {
				server.locations.push_back(location);
				inLocation = false;
				blockDepth = 1;
			}
			else if (blockDepth == 1) {
				servers.push_back(server);
				inServer = false;
				blockDepth = 0;
			}
			else {
				std::cerr << "Erreur : fermeture de bloc inattendue." << std::endl;
				return false;
			}
			continue;
		}

		//fonction assignement
		std::string key = title;
		std::string value, word;
		while (iss >> word) {
			if (!value.empty())
				value += " ";
			value += word;
		}

		if (key.empty() || value.empty())
			continue;
		if (inLocation)
			location.instruct[key] = value;
		else if (inServer)
			server.instruct[key] = value;
	}

	if (blockDepth != 0) {
		std::cerr << "Erreur : bloc non fermé correctement." << std::endl;
		return false;
	}
	return true;
}

// Affiche le résultat
void printConfig(const std::vector<Server>& servers) {
	for (std::size_t s = 0; s < servers.size(); ++s) {
		std::cout << "\n--- SERVER " << s + 1 << " ---" << std::endl;

		for (std::map<std::string, std::string>::const_iterator it = servers[s].instruct.begin();
			 it != servers[s].instruct.end(); ++it) {
			std::cout << "  " << it->first << " → " << it->second << std::endl;
		}

		for (std::size_t i = 0; i < servers[s].locations.size(); ++i) {
			std::cout << "  Location: " << servers[s].locations[i].path << std::endl;
			for (std::map<std::string, std::string>::const_iterator lit = servers[s].locations[i].instruct.begin();
				 lit != servers[s].locations[i].instruct.end(); ++lit) {
				std::cout << "    " << lit->first << " → " << lit->second << std::endl;
			}
		}
	}
}

// Fonction principale
int main(int ac, char** av) {
	if (ac < 2) {
		std::cerr << "Usage: ./a.out <file>" << std::endl;
		return 1;
	}

	std::ifstream file;
	if (!openConfigFile(av[1], file))
		return 1;

	std::vector<Server> servers;
	if (!parseConfig(file, servers)) {
		file.close();
		return 1;
	}
	file.close();

	printConfig(servers);
	return 0;
}
