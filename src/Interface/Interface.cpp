/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Interface.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 12:01:41 by pharbst           #+#    #+#             */
/*   Updated: 2024/01/20 21:07:30 by pharbst          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Interface.hpp"

std::map<std::string, protocolFunction>	Interface::_protocolMap;
std::map<int, std::string>				Interface::_outputBuffer;

void	Interface::interface(int sock, t_data sockData) {
	// std::cout << "interface called" << std::endl;
	if (sockData.read) {
		std::string		request;
		std::string		response;
		#if defined(__SSL__)
		if (sockData.ssl) {
			if (sslReadFromSocket(sock, request, sockData)) {
				std::cout << "readFromSocket failed" << std::endl;
				socketManager::removeSocket(sock);
				return ;	// remove client
			}
		}
		else {
			if (readFromSocket(sock, request)) {
				std::cout << "readFromSocket failed" << std::endl;
				socketManager::removeSocket(sock);
				return ;	// remove client
			}
		}
		#else
		if (readFromSocket(sock, request)) {
			std::cout << "readFromSocket failed" << std::endl;
			socketManager::removeSocket(sock);
			return ;	// remove client
		}
		#endif
		if (passRequest(request, response)) {
			std::cout << "passRequest failed" << std::endl;
			socketManager::removeSocket(sock);
			return ;	// remove client
		}
		std::cout << "response added to buffer for socket: " << sock << std::endl;
		_outputBuffer.insert(std::pair<int, std::string>(sock, response));
	}
	if (sockData.write && _outputBuffer.find(sock) != _outputBuffer.end()) {
		std::cout << "writing to socket: " << sock << std::endl;
		std::string		response = _outputBuffer[sock];
		#if defined(__SSL__)
		if (sockData.ssl) {
			if (sslWriteToSocket(sock, response, sockData)) {
				std::cout << "writeToSocket failed" << std::endl;
				socketManager::removeSocket(sock);
				_outputBuffer.erase(sock);
				return ;	// remove client
			}
		}
		else {
			if (writeToSocket(sock, response)) {
				std::cout << "writeToSocket failed" << std::endl;
				socketManager::removeSocket(sock);
				_outputBuffer.erase(sock);
				return ;	// remove client
			}
		}
		#else
		if (writeToSocket(sock, response)) {
			std::cout << "writeToSocket failed" << std::endl;
			socketManager::removeSocket(sock);
			_outputBuffer.erase(sock);
			return ;	// remove client
		}
		#endif
		_outputBuffer.erase(sock);
	}
}

void	Interface::addProtocol(std::string protocol, protocolFunction function) {
	_protocolMap.insert(std::pair<std::string, protocolFunction>(protocol, function));
}
