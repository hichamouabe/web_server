NAME_LEXER  = test_lexer
NAME_PARSER = test_parser

CC          = c++
CFLAGS      = -Wall -Wextra -Werror -std=c++98

# --- Sources for Lexer Test (Lexer + main) ---
SRCS_LEXER  = Lexer.cpp main.cpp

# --- Sources for Parser Test (Everything except main.cpp) ---
SRCS_PARSER = Lexer.cpp Parser.cpp ConfigNode.cpp test_main.cpp

# --- Rules ---

all: $(NAME_PARSER)

# Target to compile the Lexer test
lexer: $(SRCS_LEXER)
	$(CC) $(CFLAGS) $(SRCS_LEXER) -o $(NAME_LEXER)
	@echo "Compiled $(NAME_LEXER) (Lexer Only)"

# Target to compile the Parser test
parser: $(SRCS_PARSER)
	$(CC) $(CFLAGS) $(SRCS_PARSER) -o $(NAME_PARSER)
	@echo "Compiled $(NAME_PARSER) (Lexer + Parser)"

clean:
	rm -f $(NAME_LEXER) $(NAME_PARSER)

fclean: clean

re: fclean all

.PHONY: all lexer parser clean fclean re
