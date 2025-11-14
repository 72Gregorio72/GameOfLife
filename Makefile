# Conway's Life - Balatro Edition - Makefile

# Compilatori
CXX_LINUX = g++
CXX_WINDOWS = x86_64-w64-mingw32-g++

# Directory
SRC_DIR = src
INC_DIR = include
ASSETS_DIR = assets
FONT_DIR = $(ASSETS_DIR)/fonts
BUILD_DIR = build
BUILD_DIR_WIN = build_windows
DIST_DIR = dist
DIST_LINUX = $(DIST_DIR)/linux
DIST_WINDOWS = $(DIST_DIR)/windows
SCRIPTS_DIR = scripts

# Flag di compilazione
CXXFLAGS = -std=c++17 -Wall -O2
INCLUDES = -I$(INC_DIR)

# Percorsi SFML per MinGW
MINGW_PREFIX = /usr/x86_64-w64-mingw32
SFML_INCLUDE = -I$(MINGW_PREFIX)/include
SFML_LIB = -L$(MINGW_PREFIX)/lib

# Flag di linking
LDFLAGS_LINUX = -lsfml-graphics -lsfml-window -lsfml-system
LDFLAGS_WINDOWS = $(SFML_LIB) -static-libgcc -static-libstdc++ \
                  -lsfml-graphics-s -lsfml-window-s -lsfml-system-s \
                  -lopengl32 -lwinmm -lgdi32 -lfreetype -DSFML_STATIC

# File sorgenti
SOURCES = main.cpp revive.cpp game.cpp
OBJECTS_LINUX = $(SOURCES:%.cpp=$(BUILD_DIR)/%.o)
OBJECTS_WINDOWS = $(SOURCES:%.cpp=$(BUILD_DIR_WIN)/%.o)

# Eseguibili
TARGET_LINUX = game_of_life
TARGET_WINDOWS = game_of_life.exe

# Colori per output
RED = \033[0;31m
GREEN = \033[0;32m
YELLOW = \033[1;33m
BLUE = \033[0;34m
NC = \033[0m

# Regola di default
all: linux

# Compilazione per Linux
linux: $(BUILD_DIR) $(TARGET_LINUX)
	@echo -e "$(GREEN)✓ Linux build completed!$(NC)"

$(TARGET_LINUX): $(OBJECTS_LINUX)
	@echo -e "$(BLUE)Linking $(TARGET_LINUX)...$(NC)"
	$(CXX_LINUX) $(OBJECTS_LINUX) -o $@ $(LDFLAGS_LINUX)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo -e "$(YELLOW)Compiling $<...$(NC)"
	$(CXX_LINUX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Cross-compilazione per Windows
windows: check-sfml check-font $(BUILD_DIR_WIN) $(TARGET_WINDOWS)
	@echo -e "$(GREEN)✓ Windows build completed!$(NC)"

$(TARGET_WINDOWS): $(OBJECTS_WINDOWS)
	@echo -e "$(BLUE)Linking $(TARGET_WINDOWS)...$(NC)"
	$(CXX_WINDOWS) $(OBJECTS_WINDOWS) -o $@ $(LDFLAGS_WINDOWS)

$(BUILD_DIR_WIN)/%.o: $(SRC_DIR)/%.cpp
	@echo -e "$(YELLOW)Compiling $< for Windows...$(NC)"
	$(CXX_WINDOWS) $(CXXFLAGS) $(INCLUDES) $(SFML_INCLUDE) -DSFML_STATIC -c $< -o $@

# Package per distribuzione
dist-linux: linux
	@echo -e "$(BLUE)Creating Linux distribution package...$(NC)"
	@mkdir -p $(DIST_LINUX)
	@cp $(TARGET_LINUX) $(DIST_LINUX)/
	@mkdir -p $(DIST_LINUX)/assets/fonts
	@cp $(FONT_DIR)/* $(DIST_LINUX)/assets/fonts/ 2>/dev/null || \
		cp /usr/share/fonts/truetype/dejavu/DejaVuSans.ttf $(DIST_LINUX)/assets/fonts/font.ttf
	@cp docs/README*.md $(DIST_LINUX)/ 2>/dev/null || true
	@echo -e "$(GREEN)✓ Linux package ready in $(DIST_LINUX)$(NC)"

dist-windows: windows
	@echo -e "$(BLUE)Creating Windows distribution package...$(NC)"
	@mkdir -p $(DIST_WINDOWS)
	@cp $(TARGET_WINDOWS) $(DIST_WINDOWS)/
	@mkdir -p $(DIST_WINDOWS)/assets/fonts
	@cp $(FONT_DIR)/* $(DIST_WINDOWS)/assets/fonts/ 2>/dev/null || \
		echo -e "$(YELLOW)Warning: No fonts found in $(FONT_DIR)$(NC)"
	@cp docs/README*.md $(DIST_WINDOWS)/ 2>/dev/null || true
	@echo -e "$(GREEN)✓ Windows package ready in $(DIST_WINDOWS)$(NC)"
	@echo -e "$(YELLOW)Remember to include font.ttf in assets/fonts/$(NC)"

dist: dist-linux dist-windows
	@echo -e "$(GREEN)✓ All distribution packages created!$(NC)"

# Verifica SFML per Windows
check-sfml:
	@if [ ! -d "$(MINGW_PREFIX)/include/SFML" ]; then \
		echo -e "$(RED)✗ SFML for Windows not found!$(NC)"; \
		echo -e "$(YELLOW)Run: make setup-sfml$(NC)"; \
		exit 1; \
	fi

# Verifica font
check-font:
	@if [ ! -f "$(FONT_DIR)/font.ttf" ]; then \
		echo -e "$(YELLOW)⚠ Font not found. Downloading...$(NC)"; \
		$(MAKE) download-font; \
	fi

# Download font
download-font:
	@echo -e "$(BLUE)Downloading font...$(NC)"
	@mkdir -p $(FONT_DIR)
	@chmod +x $(SCRIPTS_DIR)/download_font.sh 2>/dev/null || true
	@if [ -f "$(SCRIPTS_DIR)/download_font.sh" ]; then \
		cd $(FONT_DIR) && ../../$(SCRIPTS_DIR)/download_font.sh; \
	else \
		cp /usr/share/fonts/truetype/dejavu/DejaVuSans.ttf $(FONT_DIR)/font.ttf 2>/dev/null || \
		echo -e "$(RED)Could not download font$(NC)"; \
	fi

# Setup SFML per cross-compilazione
setup-sfml:
	@echo -e "$(BLUE)Setting up SFML for Windows...$(NC)"
	@chmod +x $(SCRIPTS_DIR)/setup_sfml_windows.sh
	@$(SCRIPTS_DIR)/setup_sfml_windows.sh

# Setup completo
setup: setup-sfml download-font
	@echo -e "$(GREEN)✓ Setup completed!$(NC)"

# Organizza struttura progetto
organize:
	@echo -e "$(BLUE)Organizing project structure...$(NC)"
	@chmod +x $(SCRIPTS_DIR)/organize_project.sh
	@$(SCRIPTS_DIR)/organize_project.sh

# Crea directory di build
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(BUILD_DIR_WIN):
	@mkdir -p $(BUILD_DIR_WIN)

# Pulizia
clean:
	@echo -e "$(YELLOW)Cleaning build files...$(NC)"
	@rm -rf $(BUILD_DIR) $(BUILD_DIR_WIN) $(TARGET_LINUX) $(TARGET_WINDOWS)
	@echo -e "$(GREEN)✓ Clean completed!$(NC)"

# Pulizia completa
distclean: clean
	@echo -e "$(YELLOW)Cleaning distribution files...$(NC)"
	@rm -rf $(DIST_DIR)
	@echo -e "$(GREEN)✓ Distribution clean completed!$(NC)"

# Run Linux
run: linux
	@echo -e "$(BLUE)Running game...$(NC)"
	@./$(TARGET_LINUX)

# Help
help:
	@echo -e "$(BLUE)Conway's Life - Balatro Edition - Build System$(NC)"
	@echo ""
	@echo -e "$(GREEN)Build Targets:$(NC)"
	@echo "  make linux           - Build for Linux"
	@echo "  make windows         - Build for Windows"
	@echo "  make all             - Build for current platform (default: linux)"
	@echo ""
	@echo -e "$(GREEN)Distribution:$(NC)"
	@echo "  make dist-linux      - Create Linux distribution package"
	@echo "  make dist-windows    - Create Windows distribution package"
	@echo "  make dist            - Create both distributions"
	@echo ""
	@echo -e "$(GREEN)Setup:$(NC)"
	@echo "  make setup           - Complete setup (SFML + fonts)"
	@echo "  make setup-sfml      - Install SFML for Windows"
	@echo "  make download-font   - Download font file"
	@echo "  make organize        - Organize project structure"
	@echo ""
	@echo -e "$(GREEN)Utilities:$(NC)"
	@echo "  make run             - Build and run (Linux)"
	@echo "  make clean           - Remove build files"
	@echo "  make distclean       - Remove build and dist files"
	@echo "  make help            - Show this help"
	@echo ""
	@echo -e "$(YELLOW)First time setup:$(NC)"
	@echo "  1. make organize     - Organize files"
	@echo "  2. make setup        - Setup dependencies"
	@echo "  3. make windows      - Build for Windows"
	@echo "  4. make dist         - Create packages"

.PHONY: all linux windows dist dist-linux dist-windows clean distclean \
        setup setup-sfml download-font organize check-sfml check-font run help