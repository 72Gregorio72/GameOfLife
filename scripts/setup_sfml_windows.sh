#!/bin/bash

echo "=== Setup SFML for Windows Cross-Compilation ==="

# Colori
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Verifica MinGW
if ! command -v x86_64-w64-mingw32-g++ &> /dev/null; then
    echo -e "${YELLOW}Installing MinGW...${NC}"
    sudo apt-get update
    sudo apt-get install -y mingw-w64 g++-mingw-w64-x86-64 wget unzip
fi

# Crea directory temporanea
TEMP_DIR="temp_sfml_setup"
mkdir -p $TEMP_DIR
cd $TEMP_DIR

echo -e "${YELLOW}Downloading SFML 2.6.1 for MinGW...${NC}"
wget -q --show-progress https://www.sfml-dev.org/files/SFML-2.6.1-windows-gcc-13.1.0-mingw-64-bit.zip

if [ $? -ne 0 ]; then
    echo -e "${RED}Download failed! Trying alternative source...${NC}"
    wget -q --show-progress https://github.com/SFML/SFML/releases/download/2.6.1/SFML-2.6.1-windows-gcc-13.1.0-mingw-64-bit.zip
fi

echo -e "${YELLOW}Extracting...${NC}"
unzip -q SFML-2.6.1-windows-gcc-13.1.0-mingw-64-bit.zip

# Trova la directory estratta
SFML_DIR=$(find . -maxdepth 1 -type d -name "SFML-*" | head -n 1)

if [ -z "$SFML_DIR" ]; then
    echo -e "${RED}Failed to extract SFML!${NC}"
    exit 1
fi

echo -e "${YELLOW}Installing SFML to system directories...${NC}"

# Crea directory se non esistono
sudo mkdir -p /usr/x86_64-w64-mingw32/include
sudo mkdir -p /usr/x86_64-w64-mingw32/lib

# Copia header files
echo "Copying headers..."
sudo cp -r $SFML_DIR/include/SFML /usr/x86_64-w64-mingw32/include/

# Copia librerie
echo "Copying libraries..."
sudo cp -r $SFML_DIR/lib/* /usr/x86_64-w64-mingw32/lib/

# Pulizia
cd ..
rm -rf $TEMP_DIR

echo -e "${GREEN}SFML installed successfully!${NC}"
echo -e "${GREEN}You can now run: make windows${NC}"
