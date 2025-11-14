
A roguelike twist on Conway's Game of Life with risk/reward mechanics.

## Project Structure

```
gameOfLife/
├── src/              # Source files (.cpp)
├── include/          # Header files (.hpp)
├── assets/           # Game assets
│   └── fonts/       # Font files
├── build/           # Linux build output
├── build_windows/   # Windows build output
├── dist/            # Distribution packages
│   ├── linux/       # Linux release
│   └── windows/     # Windows release
├── scripts/         # Build scripts
└── docs/            # Documentation
```

## Quick Start

### First Time Setup
```bash
# Organize project structure
make organize

# Setup dependencies (SFML + fonts)
make setup

# Build for Windows
make windows

# Create distribution packages
make dist
```

### Build Commands
- `make linux` - Build for Linux
- `make windows` - Build for Windows
- `make dist` - Create distribution packages
- `make run` - Build and run (Linux)
- `make clean` - Clean build files
- `make help` - Show all commands

## Distribution

After running `make dist`, you'll find:
- `dist/linux/game_of_life` - Linux executable + assets
- `dist/windows/game_of_life.exe` - Windows executable + assets

Both packages include the necessary fonts in `assets/fonts/`.

## Controls

- **Space** - Start/Pause simulation
- **1-4** - Select cell type
- **Click** - Place cells
- **E** - Toggle erase mode
- **C** - Clear grid
- **R** - Reset zoom/pan
- **N** - Next blind (after winning)
- **Mouse Wheel** - Zoom
- **Right Click + Drag** - Pan view

## Game Mechanics

### Cell Types
1. **Normal** - $1, 0% random death
2. **Risky** - $3, 10% random death  
3. **Very Risky** - $8, 30% random death
4. **Super Risky** - $20, 50% random death

### Fatigue System
Cells that survive multiple consecutive generations accumulate fatigue:
- Each generation alive adds 5% death chance (increases per Ante)
- Prevents infinite patterns
- Scales with difficulty

See `docs/` for detailed documentation.
