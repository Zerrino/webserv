# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: marvin <marvin@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/11/14 15:23:12 by gdelvign          #+#    #+#              #
#    Updated: 2024/11/14 21:34:49 by marvin           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC			:= c++
CPPFLAGS	:= -Wall -Wextra -Werror -std=c++98
SRC_DIR		:= sources/
BUILD_DIR	:= .build/
SRC			:= $(addsuffix .cpp, $(addprefix $(SRC_DIR), main ConfigParser Socket SendToClient ClientRequest Cookie))
OBJ			:= $(addprefix $(BUILD_DIR), $(notdir $(SRC:.cpp=.o)))
DEP			:= $(OBJ:.o=.d)
NAME		:= webserv

# Cible principale
all: $(NAME)

# Compilation de l'exécutable à partir des objets
$(NAME): $(OBJ)
	@$(CC) $(CPPFLAGS) -o $@ $(OBJ)
	@printf "Compiling..."
	@sleep 1
	@printf "\r$(NAME) compiled successfully!\n"

# Inclusion des fichiers de dépendances
-include $(DEP)

# Compilation des fichiers objets et génération des fichiers de dépendances
$(BUILD_DIR)%.o: $(SRC_DIR)%.cpp | MKDIR
	@$(CC) $(CPPFLAGS) -MMD -c $< -o $@

# Création du répertoire de build si nécessaire
MKDIR:
	@mkdir -p $(BUILD_DIR)

# Nettoyage des objets et des fichiers de dépendances
clean:
	@rm -rf $(BUILD_DIR)
	@printf "$(BUILD_DIR) folder removed.\n"

# Nettoyage complet
fclean: clean
	@rm -rf $(NAME)
	@printf "$(NAME) deleted.\nProject fully cleaned.\n"

# Recompilation complète
re: fclean all

.PHONY: all clean fclean re
