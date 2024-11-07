/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdelvign <gdelvign@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 12:07:59 by gdelvign          #+#    #+#             */
/*   Updated: 2024/11/07 22:05:25 by gdelvign         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <unistd.h>
#include <fstream>
#include <sys/stat.h>
#include "ConfigLexer.hpp"

#define BACKLOG 128
#define BUFFER_SIZE 1024
#define DEFAULT_CONFIG_PATH "Config/default.conf"

enum TokenType
{
	Directive,
	Variable,
	Value,
	Operator,
	Punctuation,
	Regex,
};

struct Token
{
	std::string value;
	TokenType type;
};

std::vector<Token> tokenize(std::ifstream &file);
std::vector<std::string> splitInWords(std::ifstream &file);