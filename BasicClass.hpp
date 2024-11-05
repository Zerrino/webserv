/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BasicClass.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Zerrino <Zerrino@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 20:34:10 by Zerrino           #+#    #+#             */
/*   Updated: 2024/11/04 20:34:10 by Zerrino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BASICCLASS_HPP
# define BASICCLASS_HPP

# include <iostream>

class	BasicClass
{
	private:

	public:
		BasicClass();
		~BasicClass();
		BasicClass(const BasicClass &cp);
		BasicClass& operator= (const BasicClass &cp);
};

#endif
