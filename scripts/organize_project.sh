#!/bin/bash

echo "=== Organizing Project Structure ==="

# Crea le directory necessarie
mkdir -p src
mkdir -p include
mkdir -p assets/fonts
mkdir -p build
mkdir -p build_windows
mkdir -p dist/linux
mkdir -p dist/windows
mkdir -p scripts
mkdir -p docs

# Sposta file sorgenti in src/
if [ -f main.cpp ]; then
    mv main.cpp src/ 2>/dev/null || true
fi
if [ -f revive.cpp ]; then
    mv revive.cpp src/ 2>/dev/null || true
fi
if [ -f game.cpp ]; then
    mv game.cpp src/ 2>/dev/null || true
fi

# Sposta header in include/ se non è già una directory
if [ -f life.hpp ]; then
    mv life.hpp include/ 2>/dev/null || true
fi

# Sposta font in assets/fonts/
if [ -f font.ttf ]; then
    mv font.ttf assets/fonts/ 2>/dev/null || true
fi
if [ -f arial.ttf ]; then
    mv arial.ttf assets/fonts/ 2>/dev/null || true
fi

# Sposta script in scripts/
for script in *.sh; do
    if [ -f "$script" ] && [ "$script" != "organize_project.sh" ]; then
        mv "$script" scripts/ 2>/dev/null || true
    fi
done

# Sposta documentazione in docs/
for doc in README*.md; do
    if [ -f "$doc" ]; then
        mv "$doc" docs/ 2>/dev/null || true
    fi
done

echo "Project structure organized!"
tree -L 2 -I 'build*' || ls -R
