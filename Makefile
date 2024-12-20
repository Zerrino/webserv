# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: gdelvign <gdelvign@student.s19.be>         +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: Invalid date        by                   #+#    #+#              #
#    Updated: 2024/12/20 17:01:53 by gdelvign         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


CC			:= c++
CPPFLAGS	:= -std=c++98 -Wall -Wextra -Werror
SRC_DIR		:= sources/
BUILD_DIR	:= .build/
SRC			:= $(addsuffix .cpp, $(addprefix $(SRC_DIR), main ConfigParser Socket SendToClient ClientRequest Cookie Request ParseHttp CGI CGIRequest CGIUtils))
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
