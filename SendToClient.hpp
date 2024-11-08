/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SendToClient.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 22:20:30 by zerrino           #+#    #+#             */
/*   Updated: 2024/11/08 22:28:22 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SENDTOCLIENT_HPP
# define SENDTOCLIENT_HPP
# include "includes/webserv.hpp"

class	SendToClient
{
	private:

	protected:
		SendToClient();
	public:
	void	SayHey();
	~SendToClient();
	SendToClient(const SendToClient &cp);
	SendToClient& operator= (const SendToClient &cp);
};

#endif
