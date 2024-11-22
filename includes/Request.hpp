/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/15 05:56:27 by marvin            #+#    #+#             */
/*   Updated: 2024/11/15 05:56:27 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP
# include "webserv.hpp"
# include "Cookie.hpp"

class	Request
{
	private:
	std::map<std::string, std::string>	_requestMap;
	std::string _this_file;
	public:
		Request();
		~Request();
		Request(const Request &cp);
		Request& operator= (const Request &cp);
		void	parseContent(std::map<std::string, std::string> &myMap);
		std::map<std::string, std::string>	parseRequest(std::string request);
		std::map<std::string, std::string>	reParseRequest(std::map<std::string, std::string> myMap);
		std::map<std::string, std::string>	parseInd(std::string key, std::string content);
		std::map<std::string, std::string>	mergeMap(std::map<std::string, std::string> myMap, std::map<std::string, std::string> mySecondMap);
		std::map<std::string, std::string>	reformat(std::map<std::string, std::string> myMap);
		void	printMap(std::map<std::string, std::string> myMap);
		std::string urlParsing(std::vector<std::string> listLocation, std::string url);
};

#endif
