NAME_LEXER      = test_lexer
NAME_PARSER     = test_parser
NAME_VALIDATOR  = validator_test  # <--- New Executable Name

CC              = c++
CFLAGS          = -Wall -Wextra -Werror -std=c++98

# --- Phase 1: Lexer Sources ---
SRCS_LEXER      = Lexer.cpp main.cpp

# --- Phase 2: Parser Sources ---
SRCS_PARSER     = Lexer.cpp Parser.cpp ConfigNode.cpp test_main.cpp

# --- Phase 3: Validator Sources (New) ---
SRCS_VALIDATOR  = Lexer.cpp Parser.cpp ConfigNode.cpp ConfigValidator.cpp test_validator.cpp

# --- Rules ---

all: $(NAME_PARSER) $(NAME_VALIDATOR)

# 1. Target for Lexer (Basic)
lexer: $(SRCS_LEXER)
	$(CC) $(CFLAGS) $(SRCS_LEXER) -o $(NAME_LEXER)
	@echo "✅ Compiled $(NAME_LEXER)"

# 2. Target for Parser (Tree Building)
parser: $(SRCS_PARSER)
	$(CC) $(CFLAGS) $(SRCS_PARSER) -o $(NAME_PARSER)
	@echo "✅ Compiled $(NAME_PARSER)"

# 3. Target for Validator (Semantic Check) - THIS IS THE NEW ONE
validator: $(SRCS_VALIDATOR)
	$(CC) $(CFLAGS) $(SRCS_VALIDATOR) -o $(NAME_VALIDATOR)
	@echo "✅ Compiled $(NAME_VALIDATOR)"

clean:
	rm -f $(NAME_LEXER) $(NAME_PARSER) $(NAME_VALIDATOR)

fclean: clean

re: fclean all

.PHONY: all lexer parser validator clean fclean re
