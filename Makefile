NAME_LEXER      = test_lexer
NAME_PARSER     = test_parser
NAME_VALIDATOR  = validator_test
NAME_LOADER     = test_loader    # <--- NEW TARGET NAME

CC              = c++
CFLAGS          = -Wall -Wextra -Werror -std=c++98

# --- Sources ---
SRCS_LEXER      = Lexer.cpp main.cpp
SRCS_PARSER     = Lexer.cpp Parser.cpp ConfigNode.cpp test_main.cpp
SRCS_VALIDATOR  = Lexer.cpp Parser.cpp ConfigNode.cpp ConfigValidator.cpp test_validator.cpp Utils.cpp

# --- NEW: Sources for the Loader Test ---
# Includes EVERYTHING: Lexer, Parser, Validator, Loader, and the new Test Main
SRCS_LOADER     = Lexer.cpp Parser.cpp ConfigNode.cpp ConfigValidator.cpp ConfigLoader.cpp test_loader.cpp Utils.cpp

# --- Rules ---

all: $(NAME_LOADER)

# 1. Lexer
lexer: $(SRCS_LEXER)
	$(CC) $(CFLAGS) $(SRCS_LEXER) -o $(NAME_LEXER)

# 2. Parser
parser: $(SRCS_PARSER)
	$(CC) $(CFLAGS) $(SRCS_PARSER) -o $(NAME_PARSER)

# 3. Validator
validator: $(SRCS_VALIDATOR)
	$(CC) $(CFLAGS) $(SRCS_VALIDATOR) -o $(NAME_VALIDATOR)

# 4. Loader (The Final Step)
loader: $(SRCS_LOADER)
	$(CC) $(CFLAGS) $(SRCS_LOADER) -o $(NAME_LOADER)
	@echo "✅ Compiled $(NAME_LOADER) - Ready to generate configs!"

clean:
	rm -f $(NAME_LEXER) $(NAME_PARSER) $(NAME_VALIDATOR) $(NAME_LOADER)

fclean: clean

re: fclean all

.PHONY: all lexer parser validator loader clean fclean re
