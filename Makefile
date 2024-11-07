# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: marvin <marvin@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/05/06 22:30:05 by alexafer          #+#    #+#              #
#    Updated: 2024/11/05 16:18:18 by Zerrino          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


CXX = c++
CXXFLAGS = -std=c++98 -Wall -Wextra -Werror

OBJ_DIR = .objs

TARGET = program

MAIN = main

HPP_FILES =

#FILES
FILES += ClientRequest
FILES += Socket

#INCLUDE

SOURCES = $(MAIN:=.cpp) $(FILES:=.cpp)

HEADER  = $(FILES:=.hpp) $(HPP_FILES:=.hpp)

OBJECTS = $(addprefix $(OBJ_DIR)/, $(SOURCES:.cpp=.o))


all: $(TARGET)

$(TARGET): $(OBJECTS) $(HEADER)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET)

$(OBJ_DIR)/%.o: %.cpp $(HEADER)
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS)
	@if [ -d $(OBJ_DIR) ]; then rmdir $(OBJ_DIR); fi

fclean: clean
	rm -f $(TARGET)

re: fclean all

header/%:
		@vim -c 'Stdheader' -c 'wq' $*


include:
	@for name in $(filter-out $@,$(MAKECMDGOALS)); do \
		$(MAKE) update_includes NAME=$$name; \
	done

create:
	@for name in $(filter-out $@,$(MAKECMDGOALS)); do \
		$(MAKE) update_file NAME=$$name; \
		$(MAKE) create_files NAME=$$name; \
	done

update_file:
	@grep -qxF "FILES += $(NAME)" Makefile || (awk '/#FI\
	LES/ {print; print "FILES += $(NAME)"; next}1' Makefile > Makefile.tmp && mv Makefile.tmp Makefile)

update_includes:
	@grep -qxF "INCLUDE += $(NAME)" Makefile || (awk '/#INC\
	LUDE/ {print; print "INCLUDE += $(NAME)"; next}1' Makefile > Makefile.tmp && mv Makefile.tmp Makefile)

create_files:
	@if [ ! -f "$(NAME).hpp" ]; then \
		echo "#ifndef $$(echo $(NAME) | tr 'a-z' 'A-Z')_HPP" >> $(NAME).hpp; \
		echo "# define $$(echo $(NAME) | tr 'a-z' 'A-Z')_HPP" >> $(NAME).hpp; \
		echo "" >> $(NAME).hpp; \
		for inc in $(INCLUDE); do \
			echo "# include <$$inc>" >> $(NAME).hpp; \
		done; \
		echo "" >> $(NAME).hpp; \
		echo -e "class\t$(NAME)" >> $(NAME).hpp; \
		echo -e "{" >> $(NAME).hpp; \
		echo -e "\tprivate:" >> $(NAME).hpp; \
		echo "" >> $(NAME).hpp; \
		echo -e "\tpublic:" >> $(NAME).hpp; \
		echo -e "\t\t$(NAME)();" >> $(NAME).hpp; \
		echo -e "\t\t~$(NAME)();" >> $(NAME).hpp; \
		echo -e "\t\t$(NAME)(const $(NAME) &cp);" >> $(NAME).hpp; \
		echo -e "\t\t$(NAME)& operator= (const $(NAME) &cp);" >> $(NAME).hpp; \
		echo "};" >> $(NAME).hpp; \
		echo "" >> $(NAME).hpp; \
		echo "#endif" >> $(NAME).hpp; \
		make header/$(NAME).hpp; \
	fi

	@if [ ! -f "$(NAME).cpp" ]; then \
		echo "#include \"$(NAME).hpp\"" >> $(NAME).cpp; \
		echo "" >> $(NAME).cpp; \
		echo "$(NAME)::$(NAME)()" >> $(NAME).cpp; \
		echo "{" >> $(NAME).cpp; \
		echo "}" >> $(NAME).cpp; \
		echo "" >> $(NAME).cpp; \
		echo "$(NAME)::~$(NAME)()" >> $(NAME).cpp; \
		echo "{" >> $(NAME).cpp; \
		echo "}" >> $(NAME).cpp; \
		echo "" >> $(NAME).cpp; \
		echo "$(NAME)::$(NAME)(const $(NAME)& cp)" >> $(NAME).cpp; \
		echo "{" >> $(NAME).cpp; \
		echo -e "\t*this = cp;" >> $(NAME).cpp; \
		echo "}" >> $(NAME).cpp; \
		echo "" >> $(NAME).cpp; \
		echo "$(NAME)& $(NAME)::operator= (const $(NAME)& cp)" >> $(NAME).cpp; \
		echo "{" >> $(NAME).cpp; \
		echo -e "\tif (this != &cp)" >> $(NAME).cpp; \
		echo -e "\t\t*this = cp;" >> $(NAME).cpp; \
		echo -e "\treturn (*this);" >> $(NAME).cpp; \
		echo "}" >> $(NAME).cpp; \
		make header/$(NAME).cpp; \
	fi

main:
	@if [ ! -f "main.cpp" ]; then \
		for file in $(FILES); do \
			echo "#include \"$$file.hpp\"" >> main.cpp; \
		done; \
		make header/main.cpp; \
		echo "" >> main.cpp; \
		echo -e "int\tmain(void)\n{" >> main.cpp; \
		for var in $(INCLUDE); do \
			if [ "$$var" = iostream ]; then \
				echo -e "\tstd::cout << \"Hello World !\" << std::endl;" >> main.cpp; \
			fi; \
		done; \
		echo -e "\treturn (0);\n}" >> main.cpp;\
	fi

# Prevent make from interpreting the command-line arguments as targets
%:
	@:

.PHONY: all clean fclean re
