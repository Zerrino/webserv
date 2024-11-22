# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: root <root@student.42.fr>                  +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/01/02 23:04:50 by lolemmen          #+#    #+#              #
#    Updated: 2024/11/22 12:30:16 by root             ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

.SILENT:
.PHONY: NAME re all fclean clean run

NAME := btc

# Compilation

RM = rm -rf
CXX = c++
CXXFLAGS = -std=c++98 #-Wall -Werror -Wextra -Wno-shadow

SOURCES := \
	 SendToClient.cpp \
	 ClientRequest.cpp \
	 ConfigParser.cpp \
	 Cookie.cpp \
	 CGI.cpp \
	 Socket.cpp \
	 HTTPRequest.cpp \
	 CGIUtils.cpp \
	 main.cpp
	 
# **************************************************************************** #

# Special Chars

LOG_CLEAR = \033[2K
LOG_UP = \033[A
LOG_NOCOLOR = \033[0m
LOG_BLACK = \033[1;30m
LOG_RED = \033[1;31m
LOG_GREEN = \033[1;32m
LOG_YELLOW = \033[1;33m
LOG_BLUE = \033[1;34m
LOG_VIOLET = \033[1;35m
LOG_CYAN = \033[1;36m
LOG_WHITE = \033[1;37m

# **************************************************************************** #

OBJDIR := objects
OBJECTS := $(addprefix $(OBJDIR)/,$(SOURCES:.cpp=.o))
DEPENDS := $(addprefix $(OBJDIR)/,$(SOURCES:.cpp=.d))

all: $(OBJDIR) $(NAME)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(NAME): $(OBJECTS)
	echo "$(LOG_CLEAR)$(NAME)... $(LOG_CYAN)assembling... $(LOG_NOCOLOR)$(LOG_UP)"
	$(CXX) $(CXXFLAGS) $^ -o $@
	echo "$(LOG_CLEAR)$(NAME)... $(LOG_GREEN)compiled $(LOG_GREEN)✓$(LOG_NOCOLOR)"

clean:
	$(RM) $(OBJECTS) $(DEPENDS) $(OBJDIR)

fclean: clean
	$(RM) $(NAME)

re : fclean all

run: all 
		./$(NAME)

-include $(DEPENDS)

$(OBJDIR)/%.o: %.cpp Makefile
	@echo "$(LOG_CLEAR)$(NAME)... $(LOG_YELLOW)$<$(LOG_NOCOLOR)$(LOG_UP)"
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@
	