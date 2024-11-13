/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 19:57:51 by alexafer          #+#    #+#             */
/*   Updated: 2024/11/13 15:39:32 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <netinet/in.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/syscall.h>
# include <iostream>
# include <sys/socket.h>
# include <netinet/in.h>
# include <cstring>
# include <string>
# include <vector>
# include <poll.h>
# include <algorithm>
# include <ctime>
# include <fstream>
# include <map>
# include <sstream>
#include <cstdlib>

int runcgi(int fd, std::string requestType);

#endif
