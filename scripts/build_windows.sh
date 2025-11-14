#!/bin/bash

echo "=== Conway's Life - Balatro Edition - Windows Build Script ==="

# Colori per output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Verifica se MinGW è installato
if ! command -v x86_64-w64-mingw32-g++ &> /dev/null; then
    echo -e "${RED}MinGW non trovato!${NC}"
    echo -e "${YELLOW}Installazione di MinGW...${NC}"
    sudo apt-get update
    sudo apt-get install -y mingw-w64 g++-mingw-w64-x86-64
fi

# Verifica se SFML per Windows è installato
SFML_WIN_PATH="/usr/x86_64-w64-mingw32/include/SFML"
if [ ! -d "$SFML_WIN_PATH" ]; then
    echo -e "${RED}SFML per Windows non trovato!${NC}"
    echo -e "${YELLOW}Scaricamento SFML per Windows...${NC}"
    
    # Crea directory temporanea
    mkdir -p temp_sfml
    cd temp_sfml
    
    # Scarica SFML per MinGW
    wget https://www.sfml-dev.org/files/SFML-2.6.1-windows-gcc-13.1.0-mingw-64-bit.zip
    unzip SFML-2.6.1-windows-gcc-13.1.0-mingw-64-bit.zip
    
    # Copia file necessari
    sudo mkdir -p /usr/x86_64-w64-mingw32/include
    sudo mkdir -p /usr/x86_64-w64-mingw32/lib
    
    sudo cp -r SFML-2.6.1/include/* /usr/x86_64-w64-mingw32/include/
    sudo cp -r SFML-2.6.1/lib/* /usr/x86_64-w64-mingw32/lib/
    
    cd ..
    rm -rf temp_sfml
    
    echo -e "${GREEN}SFML installato con successo!${NC}"
fi

# Compila
echo -e "${YELLOW}Compilazione per Windows...${NC}"
make windows

if [ $? -eq 0 ]; then
    echo -e "${GREEN}Compilazione completata con successo!${NC}"
    echo -e "${GREEN}Eseguibile: game_of_life.exe${NC}"
    
    # Copia DLL necessarie
    echo -e "${YELLOW}Copia DLL SFML...${NC}"
    cp /usr/x86_64-w64-mingw32/lib/sfml-graphics-2.dll . 2>/dev/null || true
    cp /usr/x86_64-w64-mingw32/lib/sfml-window-2.dll . 2>/dev/null || true
    cp /usr/x86_64-w64-mingw32/lib/sfml-system-2.dll . 2>/dev/null || true
    
    echo -e "${GREEN}Build Windows completo!${NC}"
else
    echo -e "${RED}Errore durante la compilazione!${NC}"
    exit 1
fi
