# Conway's Life - Balatro Edition - Guida alla Compilazione

## Compilazione per Linux (WSL)

### Prerequisiti
```bash
sudo apt-get update
sudo apt-get install -y build-essential libsfml-dev
```

### Compilazione
```bash
make linux
./game_of_life
```

## Compilazione per Windows (da WSL)

### Metodo 1: Script automatico (raccomandato)
```bash
chmod +x build_windows.sh
./build_windows.sh
```

### Metodo 2: Manuale

#### Prerequisiti
```bash
# Installa MinGW
sudo apt-get update
sudo apt-get install -y mingw-w64 g++-mingw-w64-x86-64

# Scarica SFML per Windows
wget https://www.sfml-dev.org/files/SFML-2.6.1-windows-gcc-13.1.0-mingw-64-bit.zip
unzip SFML-2.6.1-windows-gcc-13.1.0-mingw-64-bit.zip

# Installa SFML
sudo cp -r SFML-2.6.1/include/* /usr/x86_64-w64-mingw32/include/
sudo cp -r SFML-2.6.1/lib/* /usr/x86_64-w64-mingw32/lib/
```

#### Compilazione
```bash
make windows
```

### File generati
- `game_of_life.exe` - Eseguibile per Windows
- `sfml-*.dll` - Librerie SFML necessarie (se non staticamente linkate)

## Compilazione diretta su Windows

### Prerequisiti
1. Installa [MinGW-w64](https://www.mingw-w64.org/)
2. Scarica [SFML per Windows](https://www.sfml-dev.org/download/sfml/2.6.1/)
3. Estrai SFML in `C:\SFML`

### Compilazione da Command Prompt
```cmd
g++ -std=c++17 -IC:\SFML\include main.cpp revive.cpp game.cpp -o game_of_life.exe -LC:\SFML\lib -lsfml-graphics -lsfml-window -lsfml-system
```

### Esecuzione
Assicurati che le DLL di SFML siano nella stessa directory dell'eseguibile o nel PATH:
- sfml-graphics-2.dll
- sfml-window-2.dll  
- sfml-system-2.dll

## Compilazione statica (eseguibile standalone)

Per creare un eseguibile che non richiede DLL esterne:

```bash
make windows
# oppure manualmente:
x86_64-w64-mingw32-g++ -std=c++17 -DSFML_STATIC -Iinclude main.cpp revive.cpp game.cpp \
  -o game_of_life.exe \
  -static-libgcc -static-libstdc++ \
  -lsfml-graphics-s -lsfml-window-s -lsfml-system-s \
  -lopengl32 -lwinmm -lgdi32
```

## Troubleshooting

### Errore: SFML non trovato
- Verifica che SFML sia installato correttamente
- Controlla i percorsi in LDFLAGS nel Makefile

### Errore: MinGW non trovato
```bash
make install-mingw
```

### Font non caricato
Copia un font TrueType nella directory del gioco o modifica il percorso in main.cpp:
```cpp
font.loadFromFile("arial.ttf")
```

## Pulizia

Rimuovi file di build:
```bash
make clean
```
