# Font Setup for Windows

## Option 1: Automatic Download (Recommended)

Run:
```bash
make font
```

This will download and install Roboto font automatically.

## Option 2: Manual Installation

1. Download any TrueType font (.ttf file)
2. Rename it to `font.ttf`
3. Place it in the same directory as `game_of_life.exe`

### Recommended Fonts:
- **Roboto**: https://fonts.google.com/specimen/Roboto
- **Open Sans**: https://fonts.google.com/specimen/Open+Sans
- **Ubuntu**: https://fonts.google.com/specimen/Ubuntu

## Option 3: Use Windows System Fonts

The game will automatically try to load from:
- `C:\Windows\Fonts\arial.ttf`

If you have issues, just copy any .ttf font from `C:\Windows\Fonts\` to the game directory as `font.ttf`.

## For Distribution

When distributing your game, include `font.ttf` with the executable:
```
game_of_life.exe
font.ttf
```
