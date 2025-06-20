# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ft_irc team                                +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/01/01 00:00:00 by team          #+#    #+#              #
#    Updated: 2024/01/01 00:00:00 by team         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# ================================== PROGRAM ==================================
NAME				= ft_irc

# ================================= COMPILER ==================================
CXX					= c++
CXXFLAGS			= -Wall -Wextra -Werror -std=c++98 -Wno-unused-private-field
DEPFLAGS			= -MMD -MP
LDFLAGS				= 

# Debug flags
ifdef DEBUG
	CXXFLAGS		+= -g3 -fsanitize=address
	LDFLAGS			+= -fsanitize=address
endif

# Optimization flags
ifdef OPTIMIZE
	CXXFLAGS		+= -O3
endif

# ================================== PATHS ====================================
OBJ_DIR				= obj
DEP_DIR				= $(OBJ_DIR)/.deps

# Create directories if they don't exist
$(shell mkdir -p $(OBJ_DIR) $(DEP_DIR))

# ================================= INCLUDES ==================================
INCLUDES			= -I.

# ================================== SOURCES ==================================
# All source files (based on your provided files)
SRCS				= server_integration_example.cpp \
					  Client.cpp \
					  ClientManager.cpp \
					  AuthHandler.cpp \
					  CommandParser.cpp

# If you have a separate main.cpp, uncomment and add it:
# SRCS				+= main.cpp

# If you have additional files (Server.cpp, Channel.cpp, etc.), add them here:
# SRCS				+= Server.cpp \
#					  Channel.cpp \
#					  ChannelManager.cpp

# ================================== OBJECTS ==================================
OBJS				= $(SRCS:%.cpp=$(OBJ_DIR)/%.o)
DEPS				= $(SRCS:%.cpp=$(DEP_DIR)/%.d)

# ================================== COLORS ===================================
# Color codes for pretty output
RED					= \033[0;31m
GREEN				= \033[0;32m
YELLOW				= \033[0;33m
BLUE				= \033[0;34m
MAGENTA				= \033[0;35m
CYAN				= \033[0;36m
WHITE				= \033[0;37m
RESET				= \033[0m

# Progress bar
TOTAL_FILES			:= $(words $(SRCS))
CURRENT_FILE		:= 0

# ================================== RULES ====================================
# Default rule
all: $(NAME)

# Linking
$(NAME): $(OBJS)
	@echo "$(BLUE)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(RESET)"
	@echo "$(CYAN)🔗 Linking $(NAME)...$(RESET)"
	@$(CXX) $(LDFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)✅ $(NAME) created successfully!$(RESET)"
	@echo "$(BLUE)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(RESET)"

# Compilation with progress bar
$(OBJ_DIR)/%.o: %.cpp
	@$(eval CURRENT_FILE=$(shell echo $$(($(CURRENT_FILE)+1))))
	@$(eval PERCENT=$(shell echo $$(($(CURRENT_FILE)*100/$(TOTAL_FILES)))))
	@printf "$(YELLOW)[%3d%%]$(RESET) $(CYAN)Compiling$(RESET) %-35s" "$(PERCENT)" "$<"
	@mkdir -p $(dir $@)
	@mkdir -p $(dir $(DEP_DIR)/$*.d)
	@if $(CXX) $(CXXFLAGS) $(DEPFLAGS) $(INCLUDES) -c $< -o $@ 2> /tmp/error.log; then \
		printf " $(GREEN)✓$(RESET)\n"; \
	else \
		printf " $(RED)✗$(RESET)\n"; \
		echo "$(RED)Compilation error:$(RESET)"; \
		cat /tmp/error.log; \
		rm -f /tmp/error.log; \
		exit 1; \
	fi

# Clean object files
clean:
	@echo "$(YELLOW)🧹 Cleaning object files...$(RESET)"
	@rm -rf $(OBJ_DIR)
	@echo "$(GREEN)✅ Object files cleaned!$(RESET)"

# Clean everything
fclean: clean
	@echo "$(YELLOW)🧹 Cleaning executable...$(RESET)"
	@rm -f $(NAME)
	@echo "$(GREEN)✅ Everything cleaned!$(RESET)"

# Rebuild
re: fclean all

# Debug build
debug:
	@$(MAKE) DEBUG=1 all

# Optimized build
release:
	@$(MAKE) OPTIMIZE=1 all

# Run with valgrind
valgrind: $(NAME)
	@echo "$(MAGENTA)🔍 Running with Valgrind...$(RESET)"
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
		--verbose ./$(NAME) 6667 password

# Run the server
run: $(NAME)
	@echo "$(GREEN)🚀 Starting ft_irc server...$(RESET)"
	@./$(NAME) 6667 password

# Run tests
test: $(NAME)
	@echo "$(CYAN)🧪 Running tests...$(RESET)"
	@chmod +x test_part2.sh
	@./test_part2.sh

# Format code (if you have clang-format)
format:
	@echo "$(BLUE)🎨 Formatting code...$(RESET)"
	@find . -name "*.cpp" -o -name "*.hpp" | grep -v $(OBJ_DIR) | xargs clang-format -i 2>/dev/null || echo "$(YELLOW)clang-format not found$(RESET)"
	@echo "$(GREEN)✅ Code formatted!$(RESET)"

# Count lines of code
loc:
	@echo "$(CYAN)📊 Lines of code:$(RESET)"
	@find . -name "*.cpp" -o -name "*.hpp" | grep -v $(OBJ_DIR) | xargs wc -l

# Show help
help:
	@echo "$(CYAN)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(RESET)"
	@echo "$(WHITE)                 FT_IRC MAKEFILE HELP$(RESET)"
	@echo "$(CYAN)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(RESET)"
	@echo "$(GREEN)all$(RESET)        - Build the project"
	@echo "$(GREEN)clean$(RESET)      - Remove object files"
	@echo "$(GREEN)fclean$(RESET)     - Remove object files and executable"
	@echo "$(GREEN)re$(RESET)         - Rebuild the project"
	@echo "$(GREEN)debug$(RESET)      - Build with debug flags"
	@echo "$(GREEN)release$(RESET)    - Build with optimization"
	@echo "$(GREEN)run$(RESET)        - Run the server (port 6667, password 'password')"
	@echo "$(GREEN)test$(RESET)       - Run tests"
	@echo "$(GREEN)valgrind$(RESET)   - Run with valgrind"
	@echo "$(GREEN)format$(RESET)     - Format code with clang-format"
	@echo "$(GREEN)loc$(RESET)        - Count lines of code"
	@echo "$(CYAN)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(RESET)"

# Check which files exist
check-files:
	@echo "$(CYAN)📁 Checking for source files...$(RESET)"
	@echo "$(YELLOW)Current directory: $(shell pwd)$(RESET)"
	@echo "$(YELLOW)Looking for .cpp files:$(RESET)"
	@find . -name "*.cpp" -type f | grep -v $(OBJ_DIR) || echo "$(RED)No .cpp files found!$(RESET)"
	@echo "$(YELLOW)Looking for .hpp files:$(RESET)"
	@find . -name "*.hpp" -type f | grep -v $(OBJ_DIR) || echo "$(RED)No .hpp files found!$(RESET)"

# Phony targets
.PHONY: all clean fclean re debug release run test valgrind format loc help check-files

# Include dependencies
-include $(DEPS)