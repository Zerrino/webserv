/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdelvign <gdelvign@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 20:35:07 by Zerrino           #+#    #+#             */
/*   Updated: 2024/11/05 14:54:45 by gdelvign         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BasicClass.hpp"

int main(int ac, char **av)
{
	if (ac > 2)
	{
		std::cout << "Invalid number of arguments !" << std::endl;
		return (EXIT_FAILURE);
	}
	else
	{
		if (ac == 1)
			std::cout << "Using default config file." << std::endl;
		else
			std::cout << "Config file provided is : " << std::string(av[1]) << std::endl;
	}
	return (EXIT_SUCCESS);
}
