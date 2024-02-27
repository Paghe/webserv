#include "socketManager.hpp"
#include "Interface.hpp"
#include "../include/config/Config.hpp"
#include "../include/httpTransfer/Request.hpp"
#include "../include/httpTransfer/Response.hpp"
#include "../parse/configFile.hpp"


std::string testHttp(const std::string &request/* , Config & config */) {
    // std::cout << "testHttp called" << std::endl;
    if (request.empty()) {
        std::cout << "request is empty" << std::endl;
        return "";
    } else {
		cout << MAG << "-------request----->>" << NORM << endl;
		Request		httpRequest(request);
		// cout << httpRequest << endl;
		Response	httpResponse(httpRequest);
		std::string response = httpResponse.getResponse();
		// cout << BLUE << "RESPONSE:\n" << response << NORM <<endl;
		cout << MAG << "<<------------------------" << NORM << endl;
		return response;
    }
}

void printConfig(string name, configServer server)
{
	cout << RED << "CONFIG: " << name << NORM << endl;
	std::cout << "SERVERNAME:" << " " << server._serverName << "\n";
	std::cout << "INCLUDE:" << " " << server._include << "\n";
	std::cout << "LISTEN:" << " " << server._listen << "\n";
	std::cout << "ADDRESS:" << " " << server._address << "\n";
	std::cout << "ROOT:" << " " << server._root << "\n";
	std::cout << "VALIDFORMAT:" << " " << server.validFormat << "\n";
	std::cout << "INDEX:" << " " << server._index << "\n";

	cout << ORANGE << "LOCATIONS" << NORM << endl;
	std::map<std::string, configServer::Location>::iterator it = server._locations.begin();
	for (; it != server._locations.end(); it++)
	{
		cout << "Location: " << it->first << endl;
		cout << "Name: " << it->second._name << endl;
		cout << "Default page: " << it->second._index << endl;
		cout << "Post: " << it->second._post << endl;
		cout << "Get: " << it->second._get << endl;
		cout << "Put: " << it->second._put << endl;
	}

}

int main(int argc, char **argv)
{
	static CONFIG config;
	if (!readConfig(argc, argv, config))
		return -1;
	static configServer configFile = config.begin()->second;
	printConfig(config.begin()->first, configFile);
	// int numWorker = 4;
	// parsing here
	// add application map to interface before forking the workers
	// applicationInterface::addApplication(80, /*&http::application*/);
	//Config config;
	protocolFunction testFunction = &testHttp;
	Interface::addProtocol("HTTP/1.1", testFunction);
	// add sockets
	{
		socketManager::addSocket(config.begin()->second._address, config.begin()->second._listen, IPV4, TCP);
		// socketManager::addSocket("0.0.0.0", (config.begin()++)->second._listen, IPV4, TCP);
	}
	InterfaceFunction interfaceFunction = &Interface::interface;
	socketManager::start(interfaceFunction);
	return 0;
}
