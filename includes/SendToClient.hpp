/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SendToClient.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdelvign <gdelvign@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 22:20:30 by zerrino           #+#    #+#             */
/*   Updated: 2024/12/20 17:00:36 by gdelvign         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SENDTOCLIENT_HPP
# define SENDTOCLIENT_HPP
# include "webserv.hpp"

class	SendToClient
{
	private:

	protected:
		std::string _request;
		std::string	_length;
		std::string _locationCreate;
	public:
	SendToClient();
	void	SayHey();

	std::string	requestOne(int request);
	std::string	requestTwo(int request, std::string path);
	std::string	requestThree(int request, std::string path);
	std::string	requestFour(int request, std::string path);
	std::string	requestFive(int request, std::string path);

	std::string	getDate();
	std::string getFile(std::string path);
	std::string getContentType(std::string path);

	~SendToClient();
	SendToClient(const SendToClient &cp);
	SendToClient& operator= (const SendToClient &cp);

	const std::map<std::string, std::string>& getContentTypesMap();
};

#endif
