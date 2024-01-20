#include "socketManager.hpp"
#include "Interface.hpp"
#include "../include/config/Config.hpp"
#include "../include/httpTransfer/httpTransfer.hpp"

std::string testHttp(const std::string &request/* , Config & config */) {
    // std::cout << "testHttp called" << std::endl;
    if (request.empty()) {
        std::cout << "request is empty" << std::endl;
        return "";
    } else {
		httpTransfer transfer(request);
		return transfer.exchange();
    }
}


int main()
{
	// int numWorker = 4;
	// parsing here
	// add application map to interface before forking the workers
	// applicationInterface::addApplication(80, /*&http::application*/);
	//Config config;
	protocolFunction testFunction = &testHttp;
	Interface::addProtocol("HTTP/1.1", testFunction);
	// add sockets
	{
		t_socket socket;
		socket.port = 80;
		socket.protocol = TCP;
		socket.ipVersion = IPV4;
		socket.interfaceAddress = "0.0.0.0";
		socketManager::addSocket(socket);
	}
	InterfaceFunction interfaceFunction = &Interface::interface;
	socketManager::start(interfaceFunction);
	return 0;
}