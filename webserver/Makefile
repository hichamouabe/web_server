NAME        = webserv

# Compiler and Flags
CXX         = c++
CXXFLAGS    = -Wall -Wextra -Werror  -I include

# Directories
SRC_DIR     = src
OBJ_DIR     = obj

# Source Files
# We list them explicitly to be safe and clear
SRCS        = $(SRC_DIR)/main.cpp \
              $(SRC_DIR)/config/Lexer.cpp \
              $(SRC_DIR)/config/Parser.cpp \
              $(SRC_DIR)/config/ConfigNode.cpp \
              $(SRC_DIR)/config/ConfigValidator.cpp \
              $(SRC_DIR)/config/ConfigLoader.cpp \
              $(SRC_DIR)/network/Server.cpp \
              $(SRC_DIR)/network/Client.cpp \
              $(SRC_DIR)/config/Utils.cpp

# Object Files (Mapped from SRCS)
OBJS        = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# --- Rules ---

all: $(NAME)

# Link the executable
$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo "✅ Build successful: $(NAME)"

# Compile source files into object files
# "mkdir -p" creates the directory if it doesn't exist
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)
	@echo "🧹 Cleaned object files."

fclean: clean
	rm -f $(NAME)
	@echo "🗑️  Removed executable."

re: fclean all

.PHONY: all clean fclean re
