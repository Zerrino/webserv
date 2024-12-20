/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cookie.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdelvign <gdelvign@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/14 04:06:02 by zerrino           #+#    #+#             */
/*   Updated: 2024/12/20 17:00:48 by gdelvign         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COOKIE_HPP
# define COOKIE_HPP
# include "webserv.hpp"

class	Cookie
{
	private:
		std::map<std::string, std::string> _requestData;
	protected:
		std::string	_cookiePathGlob;
	public:
		Cookie();
		~Cookie();
		Cookie(const Cookie &cp);
		Cookie& operator= (const Cookie &cp);
		std::string	createCookieId();
		std::string	isCookied(std::string path);
		std::map<std::string, std::string> getRequestData(std::string data);
		int	isRegister(std::map<std::string, std::string> myData);
		int isCookies(std::string myKey, std::string myVal, std::string CookiedID);
		void	cookiedUpdate(std::string myKey, std::string myVal, std::string CookiedID);
};

#endif
