/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdelvign <gdelvign@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 12:07:59 by gdelvign          #+#    #+#             */
/*   Updated: 2024/11/06 21:46:22 by gdelvign         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <fstream>

#define BACKLOG 128
#define BUFFER_SIZE 1024



enum TokenType
{
    Keyword,
    Identifier,
    Literal,
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