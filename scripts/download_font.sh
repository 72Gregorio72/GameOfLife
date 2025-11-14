#!/bin/bash

echo "=== Downloading Font for Game ==="

# Rimuovi file vecchi se esistono
rm -f font.ttf roboto-unhinted.zip
rm -rf roboto-unhinted

# Prova a scaricare Roboto
echo "Downloading Roboto font..."
wget -q --show-progress https://github.com/googlefonts/roboto/releases/download/v2.138/roboto-unhinted.zip

if [ $? -eq 0 ]; then
    echo "Extracting..."
    unzip -q -o roboto-unhinted.zip -d roboto-unhinted
    
    # Cerca il file Roboto-Regular.ttf in tutte le sottodirectory
    FONT_FILE=$(find roboto-unhinted -name "Roboto-Regular.ttf" -o -name "Roboto-Medium.ttf" | head -n 1)
    
    if [ -n "$FONT_FILE" ]; then
        echo "Found font: $FONT_FILE"
        cp "$FONT_FILE" font.ttf
        echo "Font successfully installed as 'font.ttf'"
    else
        echo "Roboto-Regular.ttf not found in archive. Trying alternative..."
        # Prova a prendere qualsiasi .ttf trovato
        FONT_FILE=$(find roboto-unhinted -name "*.ttf" | head -n 1)
        if [ -n "$FONT_FILE" ]; then
            cp "$FONT_FILE" font.ttf
            echo "Using alternative font: $FONT_FILE"
        else
            echo "ERROR: No TTF files found in archive!"
        fi
    fi
    
    # Pulizia
    rm -rf roboto-unhinted roboto-unhinted.zip
else
    echo "Download failed! Trying alternative method..."
    
    # Metodo alternativo: usa font dal sistema se disponibile
    if [ -f "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf" ]; then
        echo "Using DejaVu Sans from system fonts..."
        cp /usr/share/fonts/truetype/dejavu/DejaVuSans.ttf font.ttf
        echo "Font installed successfully!"
    elif [ -f "/mnt/c/Windows/Fonts/arial.ttf" ]; then
        echo "Using Arial from Windows..."
        cp /mnt/c/Windows/Fonts/arial.ttf font.ttf
        echo "Font installed successfully!"
    else
        echo "ERROR: Could not find any font!"
        echo "Please manually copy a .ttf file to this directory as 'font.ttf'"
        exit 1
    fi
fi

# Verifica che il file esista
if [ -f font.ttf ]; then
    echo "SUCCESS: font.ttf is ready!"
    ls -lh font.ttf
else
    echo "ERROR: font.ttf was not created!"
    exit 1
fi
