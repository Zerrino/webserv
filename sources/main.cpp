/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdelvign <gdelvign@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 20:12:32 by gdelvign          #+#    #+#             */
/*   Updated: 2024/12/20 20:46:33 by gdelvign         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ParseHttp.hpp"
#include "../includes/Socket.hpp"
#include "../includes/ClientRequest.hpp"
#include "../includes/SendToClient.hpp"
#include "../includes/Cookie.hpp"
#include "../includes/ConfigParser.hpp"
#include <csignal>

int main(int ac, char **av)
{
	signal(SIGPIPE, SIG_IGN);
	if (ac > 2)
		return (std::cout << "Invalid number of arguments !" << std::endl, EXIT_FAILURE);
	else
	{
		if (ac == 1)
		{
			srand((unsigned)time(NULL) * getpid());
			std::cout << "Start!" << std::endl;
			/// std::cout << cook.createCookieId() << std::endl;
			// SendToClient test;
			// std::cout << test.getFile("./data/index.html") << std::endl;

			Socket sock(AF_INET, SOCK_STREAM, 0);
			sock.runSocket(57000, 10);

			Socket sock2(AF_INET, SOCK_STREAM, 0);
			sock2.runSocket(50000, 10);

			std::vector<int> ports;
			ports.push_back(sock.get_fdSocket());
			ports.push_back(sock2.get_fdSocket());

			ClientRequest request(ports);
			setOfRuleHTTP	rules;
			HttpBlock fileConfig;
			while (true)
			{
				try
				{
					//request.pollRequest();
					request.pollExecute(rules, fileConfig);
				}
				catch (const std::exception &e)
				{
					std::cout << "error : " << e.what() << std::endl;
				}
			}
		}
		else
		{
			srand((unsigned)time(NULL) * getpid());
			std::cout << "Start!" << std::endl;
			std::vector<int> ports;
			std::vector<int> fdPorts;
			setOfRuleHTTP	rules;

			HttpBlock fileConfig;
			ConfigParser config(av[1]);
			ConfigParser::ConfigError status;
			ParseHttp	parser;

			if ((status = config.checkPathValidity()))
				return (std::cerr << config.fetchErrorMsg(status) << std::endl, EXIT_FAILURE);
			if ((status = config.open()))
				return (std::cerr << config.fetchErrorMsg(status) << std::endl, EXIT_FAILURE);
			if ((status = config.parse()))
				return (std::cerr << config.fetchErrorMsg(status) << std::endl, EXIT_FAILURE);

			fileConfig = config.getHTTP();
			ports = parser.getListenVec(fileConfig, rules);
			if (ports.size() == 0)
				throw std::runtime_error("no listend detected.");
			Socket** socketArray = new Socket*[ports.size()];

			for (std::vector<int>::iterator i = ports.begin(); i != ports.end(); ++i)
			{
				socketArray[i - ports.begin()] = new Socket(AF_INET, SOCK_STREAM, 0);
				socketArray[i - ports.begin()]->runSocket(*i, 10);

				fdPorts.push_back(socketArray[i - ports.begin()]->get_fdSocket());

			}
			ClientRequest request(fdPorts);
			//std::cout << rules.listen["57000"] << std::endl;
			parser.getDirRules(fileConfig.directives, rules, false);
			//parser.printSetOfRuleHTTP(rules);
			while (true)
			{
				try
				{
					//request.pollRequest();
					request.pollExecute(rules, fileConfig);
				}
				catch (const std::exception &e)
				{
					std::cout << "error : " << e.what() << std::endl;
				}
			}
			for (std::vector<int>::iterator i = ports.begin(); i != ports.end(); ++i)
			{
				delete socketArray[i - ports.begin()];
			}
			delete[] socketArray;
		}
	}
	return EXIT_SUCCESS;
}
