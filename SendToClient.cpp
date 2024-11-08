/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SendToClient.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 22:20:30 by zerrino           #+#    #+#             */
/*   Updated: 2024/11/08 22:28:58 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SendToClient.hpp"

SendToClient::SendToClient()
{
	std::cout << "SendToClient created!" << std::endl;
}

SendToClient::~SendToClient()
{
}

SendToClient::SendToClient(const SendToClient& cp)
{
 	*this = cp;
}

SendToClient& SendToClient::operator= (const SendToClient& cp)
{
 	if (this != &cp)
		*this = cp;
 	return (*this);
}

void	SendToClient::SayHey()
{
	std::cout << "Hey!" << std::endl;
}
