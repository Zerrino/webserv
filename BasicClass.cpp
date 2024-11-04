/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BasicClass.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Zerrino <Zerrino@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 20:34:10 by Zerrino           #+#    #+#             */
/*   Updated: 2024/11/04 20:34:10 by Zerrino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BasicClass.hpp"

BasicClass::BasicClass()
{
}

BasicClass::~BasicClass()
{
}

BasicClass::BasicClass(const BasicClass& cp)
{
	*this = cp;
}

BasicClass& BasicClass::operator= (const BasicClass& cp)
{
	if (this != &cp)
		*this = cp;
	return (*this);
}
