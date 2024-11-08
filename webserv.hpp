/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdelvign <gdelvign@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 12:07:59 by gdelvign          #+#    #+#             */
/*   Updated: 2024/11/08 13:36:55 by gdelvign         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <unistd.h>
#include <fstream>
#include <sys/stat.h>
#include <map>
#include "ConfigParser.hpp"

#define BACKLOG 128
#define BUFFER_SIZE 1024
