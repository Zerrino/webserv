/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SendToClient.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 22:20:30 by zerrino           #+#    #+#             */
/*   Updated: 2024/12/20 11:46:13 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SENDTOCLIENT_HPP
# define SENDTOCLIENT_HPP
# include "webserv.hpp"
# include "Cookie.hpp"

class	SendToClient : public Cookie
{
	private:

	protected:
		std::string _request;
		std::string	_length;
		std::string _locationCreate;
	public:
	SendToClient();
	void	SayHey();

	std::string	requestOne(int request, bool keepAlive, bool cookie);
	std::string	requestTwo(int request, std::string path, bool keepAlive, bool cookie);
	std::string	requestThree(int request, std::string path, bool keepAlive, bool cookie);
	std::string	requestFour(int request, std::string path, bool keepAlive, bool cookie);
	std::string	requestFive(int request, std::string path, bool keepAlive, bool cookie);

	std::string	getDate();
	std::string getFile(std::string path);
	std::string getContentType(std::string path);

	~SendToClient();
	SendToClient(const SendToClient &cp);
	SendToClient& operator= (const SendToClient &cp);

	const std::map<std::string, std::string>& getContentTypesMap();
};

#endif
