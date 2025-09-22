CC = c++
CFLAGS = -Wall -Werror -Wextra -std=c++98
DEBUG_FLAGS = -g

NAME = server
SRC = main.cpp \
	HTTPServer.cpp \
	Epoll.cpp \
	ParseRequest.cpp \
	ServerConf.cpp \
	Location.cpp
	ParseBodyRequest.cpp \
	ServerConf.cpp \
	Response.cpp \

OBJ_DIR = objs
OBJ = $(SRC:.cpp=.o)
OBJ_PATH = $(OBJ:%.o=$(OBJ_DIR)/%.o)

# Cible principale
all: $(NAME)

# Construction de l'exécutable en respectant les dépendances
$(NAME): $(OBJ_PATH)
	@echo "Compiling ..."
	@$(CC) $(CFLAGS) $(OBJ_PATH) -o $(NAME)
	@echo "Compiled successfully!"

# Compilation des fichiers objets
$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(OBJ_DIR)
	@$(CC) $(CFLAGS) $(DEBUG_FLAGS) -c $< -o $@

# Nettoyage des fichiers objets
clean:
	@rm -rf $(OBJ_DIR)

# Nettoyage complet
fclean: clean
	@rm -f $(NAME)

# Rebuild complet
re: fclean all