#include "life.hpp"

std::vector<std::vector<bool>> griglia(RIGHE, std::vector<bool>(COLONNE, false));
std::vector<std::vector<int>> tipoCella(RIGHE, std::vector<int>(COLONNE, 0));

extern std::vector<Blind> creaBlindList();

void disegnaCella(sf::RenderWindow& window, int riga, int colonna, float zoom, sf::Vector2f offset, int tipo) {
    float dimensioneCellaZoom = DIMENSIONE_CELLA * zoom;
    sf::RectangleShape cella(sf::Vector2f(dimensioneCellaZoom - 1, dimensioneCellaZoom - 1));
    cella.setPosition(colonna * dimensioneCellaZoom + offset.x, riga * dimensioneCellaZoom + offset.y + ALTEZZA_UI);
    cella.setFillColor(getColoreCella(tipo));
    window.draw(cella);
}

void disegnaGriglia(sf::RenderWindow& window, float zoom, sf::Vector2f offset) {
    float dimensioneCellaZoom = DIMENSIONE_CELLA * zoom;
    sf::Color coloreGriglia(40, 40, 40);
    
    for (int c = 0; c <= COLONNE; ++c) {
        sf::RectangleShape linea(sf::Vector2f(1, (ALTEZZA_FINESTRA - ALTEZZA_UI)));
        linea.setPosition(c * dimensioneCellaZoom + offset.x, ALTEZZA_UI + offset.y);
        linea.setFillColor(coloreGriglia);
        window.draw(linea);
    }
    
    for (int r = 0; r <= RIGHE; ++r) {
        sf::RectangleShape linea(sf::Vector2f(LARGHEZZA_FINESTRA, 1));
        linea.setPosition(offset.x, r * dimensioneCellaZoom + ALTEZZA_UI + offset.y);
        linea.setFillColor(coloreGriglia);
        window.draw(linea);
    }
}

void disegnaUI(sf::RenderWindow& window, GameState& gameState, Blind& blindCorrente, bool simulazioneAttiva, sf::Font& font) {
    sf::RectangleShape sfondoUI(sf::Vector2f(LARGHEZZA_FINESTRA, ALTEZZA_UI));
    sfondoUI.setPosition(0, 0);
    sfondoUI.setFillColor(sf::Color(30, 30, 35));
    window.draw(sfondoUI);
    
    sf::RectangleShape separatore(sf::Vector2f(LARGHEZZA_FINESTRA, 2));
    separatore.setPosition(0, ALTEZZA_UI);
    separatore.setFillColor(sf::Color(255, 215, 0));  // Oro
    window.draw(separatore);
    
    // Indicatore denaro
    sf::CircleShape coin(12);
    coin.setPosition(10, ALTEZZA_UI / 4 - 12);
    coin.setFillColor(sf::Color(255, 215, 0));
    window.draw(coin);
    
    // Testo denaro
    sf::Text testoDenaro;
    testoDenaro.setFont(font);
    testoDenaro.setString("$" + std::to_string(gameState.denaro));
    testoDenaro.setCharacterSize(18);
    testoDenaro.setFillColor(sf::Color::White);
    testoDenaro.setPosition(35, ALTEZZA_UI / 4 - 14);
    window.draw(testoDenaro);
    
    // Indicatore blind
    sf::RectangleShape boxBlind(sf::Vector2f(200, 30));
    boxBlind.setPosition(250, ALTEZZA_UI / 4 - 15);
    boxBlind.setFillColor(sf::Color(100, 50, 150));
    window.draw(boxBlind);
    
    // Testo blind
    sf::Text testoBlind;
    testoBlind.setFont(font);
    testoBlind.setString(blindCorrente.nome);
    testoBlind.setCharacterSize(14);
    testoBlind.setFillColor(sf::Color::White);
    testoBlind.setPosition(255, ALTEZZA_UI / 4 - 10);
    window.draw(testoBlind);
    
    // Progresso generazioni
    float progresso = std::min((float)gameState.generazioniSopravvissute / blindCorrente.obiettivo, 1.0f);
    sf::RectangleShape barraProgresso(sf::Vector2f(200 * progresso, 12));
    barraProgresso.setPosition(470, ALTEZZA_UI / 4 - 6);
    barraProgresso.setFillColor(sf::Color(0, 255, 150));
    window.draw(barraProgresso);
    
    // Outline progresso
    sf::RectangleShape outlineProgresso(sf::Vector2f(200, 12));
    outlineProgresso.setPosition(470, ALTEZZA_UI / 4 - 6);
    outlineProgresso.setFillColor(sf::Color::Transparent);
    outlineProgresso.setOutlineColor(sf::Color(100, 100, 100));
    outlineProgresso.setOutlineThickness(1);
    window.draw(outlineProgresso);
    
    // Testo progresso
    sf::Text testoProgresso;
    testoProgresso.setFont(font);
    testoProgresso.setString(std::to_string(gameState.generazioniSopravvissute) + "/" + std::to_string(blindCorrente.obiettivo));
    testoProgresso.setCharacterSize(14);
    testoProgresso.setFillColor(sf::Color::White);
    testoProgresso.setPosition(680, ALTEZZA_UI / 4 - 10);
    window.draw(testoProgresso);
    
    // Testo ricompensa
    sf::Text testoRicompensa;
    testoRicompensa.setFont(font);
    testoRicompensa.setString("Prize: $" + std::to_string(blindCorrente.ricompensa));
    testoRicompensa.setCharacterSize(14);
    testoRicompensa.setFillColor(sf::Color(255, 215, 0));
    testoRicompensa.setPosition(780, ALTEZZA_UI / 4 - 10);
    window.draw(testoRicompensa);
    
    // Celle rimanenti testo
    sf::Text testoCelle;
    testoCelle.setFont(font);
    testoCelle.setString("Cells: " + std::to_string(gameState.celleRimanenti));
    testoCelle.setCharacterSize(14);
    testoCelle.setFillColor(sf::Color::White);
    testoCelle.setPosition(920, ALTEZZA_UI / 4 - 10);
    window.draw(testoCelle);
    
    // Selezione tipo cella (seconda riga)
    std::string nomiTipi[4] = {"Normal", "Risky", "Very Risky", "Super Risky"};
    for (int tipo = 0; tipo < 4; ++tipo) {
        sf::RectangleShape box(sf::Vector2f(90, 30));
        box.setPosition(10 + tipo * 100, ALTEZZA_UI / 2 + 5);
        if (tipo == gameState.tipoCellaSelezionata) {
            box.setFillColor(getColoreCella(tipo));
        } else {
            box.setFillColor(sf::Color(50, 50, 50));
        }
        box.setOutlineColor(sf::Color::White);
        box.setOutlineThickness(tipo == gameState.tipoCellaSelezionata ? 2 : 1);
        window.draw(box);
        
        // Nome tipo
        sf::Text testoTipo;
        testoTipo.setFont(font);
        testoTipo.setString(nomiTipi[tipo]);
        testoTipo.setCharacterSize(10);
        testoTipo.setFillColor(tipo == gameState.tipoCellaSelezionata ? sf::Color::Black : sf::Color::White);
        testoTipo.setPosition(15 + tipo * 100, ALTEZZA_UI / 2 + 8);
        window.draw(testoTipo);
        
        // Valore e rischio
        sf::Text testoValori;
        testoValori.setFont(font);
        testoValori.setString("$" + std::to_string(getValoreCella(tipo)) + " | " + std::to_string((int)(getProbabilitaMorte(tipo) * 100)) + "%");
        testoValori.setCharacterSize(9);
        testoValori.setFillColor(tipo == gameState.tipoCellaSelezionata ? sf::Color::Black : sf::Color(200, 200, 200));
        testoValori.setPosition(15 + tipo * 100, ALTEZZA_UI / 2 + 20);
        window.draw(testoValori);
    }
    
    // Stato simulazione
    sf::CircleShape indicatore(6);
    indicatore.setPosition(450, ALTEZZA_UI / 2 + 15);
    indicatore.setFillColor(simulazioneAttiva ? sf::Color(0, 255, 0) : sf::Color(255, 100, 100));
    window.draw(indicatore);
    
    sf::Text testoStato;
    testoStato.setFont(font);
    testoStato.setString(simulazioneAttiva ? "Running" : (gameState.inGioco ? "Paused" : "Setup"));
    testoStato.setCharacterSize(12);
    testoStato.setFillColor(sf::Color::White);
    testoStato.setPosition(465, ALTEZZA_UI / 2 + 10);
    window.draw(testoStato);
    
    // Messaggio vittoria
    if (gameState.blindSuperato && !gameState.inGioco) {
        sf::Text testoVittoria;
        testoVittoria.setFont(font);
        testoVittoria.setString("BLIND DEFEATED! Press N for next or C to retry");
        testoVittoria.setCharacterSize(16);
        testoVittoria.setFillColor(sf::Color(0, 255, 0));
        testoVittoria.setPosition(600, ALTEZZA_UI / 2 + 10);
        window.draw(testoVittoria);
    }
}

void gestisciClick(int mouseX, int mouseY, bool modalitaCancellazione, float zoom, sf::Vector2f offset, GameState& gameState) {
    if (mouseY < ALTEZZA_UI) return;
    
    float dimensioneCellaZoom = DIMENSIONE_CELLA * zoom;
    int col = (mouseX - offset.x) / dimensioneCellaZoom;
    int row = (mouseY - ALTEZZA_UI - offset.y) / dimensioneCellaZoom;
    
    if (row >= 0 && row < RIGHE && col >= 0 && col < COLONNE) {
        if (modalitaCancellazione) {
            griglia[row][col] = false;
            tipoCella[row][col] = 0;
        } else {
            if (!griglia[row][col] && gameState.celleRimanenti > 0) {
                griglia[row][col] = true;
                tipoCella[row][col] = gameState.tipoCellaSelezionata;
                gameState.celleRimanenti--;
            }
        }
    }
}

void gestisciInputMouse(sf::Event& event, bool& isDragging, bool& isPanning, 
                        sf::Vector2i& lastMousePos, sf::RenderWindow& window,
                        bool modalitaCancellazione, float zoom, sf::Vector2f offset, GameState& gameState, bool simulazioneAttiva) {
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            // Check if clicking on cell type selector
            if (event.mouseButton.y > ALTEZZA_UI / 2 && event.mouseButton.y < ALTEZZA_UI) {
                int tipo = event.mouseButton.x / 100;
                if (tipo >= 0 && tipo < 4) {
                    gameState.tipoCellaSelezionata = tipo;
                    return;
                }
            }
            
            // Permetti di disegnare solo se la simulazione non è attiva
            if (!simulazioneAttiva) {
                isDragging = true;
                gestisciClick(event.mouseButton.x, event.mouseButton.y, modalitaCancellazione, zoom, offset, gameState);
            }
        }
        if (event.mouseButton.button == sf::Mouse::Right) {
            isPanning = true;
            lastMousePos = sf::Mouse::getPosition(window);
        }
    }
    
    if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            isDragging = false;
        }
        if (event.mouseButton.button == sf::Mouse::Right) {
            isPanning = false;
        }
    }
    
    if (isDragging && event.type == sf::Event::MouseMoved && !simulazioneAttiva) {
        gestisciClick(event.mouseMove.x, event.mouseMove.y, modalitaCancellazione, zoom, offset, gameState);
    }
}

void gestisciPanning(sf::Event& event, bool isPanning, sf::Vector2i& lastMousePos, 
                     sf::Vector2f& offset, sf::RenderWindow& window) {
    if (isPanning && event.type == sf::Event::MouseMoved) {
        sf::Vector2i currentMousePos = sf::Mouse::getPosition(window);
        sf::Vector2i delta = currentMousePos - lastMousePos;
        offset.x += delta.x;
        offset.y += delta.y;
        lastMousePos = currentMousePos;
    }
}

void gestisciZoom(sf::Event& event, float& zoom) {
    if (event.type == sf::Event::MouseWheelScrolled) {
        if (event.mouseWheelScroll.delta > 0) {
            zoom *= 1.1f;
        } else {
            zoom *= 0.9f;
        }
        if (zoom < 1) zoom = 1;
        if (zoom > 5.0f) zoom = 5.0f;
    }
}

void gestisciInputTastiera(sf::Event& event, bool& simulazioneAttiva, bool& modalitaCancellazione,
                           GameState& gameState, float& zoom, sf::Vector2f& offset, sf::Clock& clock, 
                           std::vector<Blind>& blinds) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Space) {
            if (!gameState.inGioco && gameState.celleRimanenti == 0) {
                // Inizia la simulazione
                gameState.inGioco = true;
                gameState.generazioniSopravvissute = 0;
                simulazioneAttiva = true;
                clock.restart();
            } else if (gameState.inGioco) {
                // Pausa/riprendi
                simulazioneAttiva = !simulazioneAttiva;
                if (simulazioneAttiva) clock.restart();
            }
        }
        if (event.key.code == sf::Keyboard::E) {
            modalitaCancellazione = !modalitaCancellazione;
        }
        if (event.key.code == sf::Keyboard::C) {
            for (int r = 0; r < RIGHE; ++r) {
                for (int c = 0; c < COLONNE; ++c) {
                    griglia[r][c] = false;
                    tipoCella[r][c] = 0;
                }
            }
            simulazioneAttiva = false;
            gameState.celleRimanenti = blinds[gameState.blindCorrente].celleDisponibili;
            gameState.inGioco = false;
            gameState.generazioniSopravvissute = 0;
        }
        if (event.key.code == sf::Keyboard::R) {
            zoom = 1.0f;
            offset = sf::Vector2f(0.0f, 0.0f);
        }
        if (event.key.code == sf::Keyboard::N && !gameState.inGioco && gameState.blindSuperato) {
            // Next blind - solo se hai vinto
            if (gameState.blindCorrente < (int)blinds.size() - 1) {
                gameState.blindCorrente++;
                gameState.celleRimanenti = blinds[gameState.blindCorrente].celleDisponibili;
                gameState.generazioniSopravvissute = 0;
                gameState.blindSuperato = false;
                for (int r = 0; r < RIGHE; ++r) {
                    for (int c = 0; c < COLONNE; ++c) {
                        griglia[r][c] = false;
                        tipoCella[r][c] = 0;
                    }
                }
            }
        }
        // Selezione tipo cella con numeri
        if (event.key.code == sf::Keyboard::Num1) gameState.tipoCellaSelezionata = 0;
        if (event.key.code == sf::Keyboard::Num2) gameState.tipoCellaSelezionata = 1;
        if (event.key.code == sf::Keyboard::Num3) gameState.tipoCellaSelezionata = 2;
        if (event.key.code == sf::Keyboard::Num4) gameState.tipoCellaSelezionata = 3;
    }
}

int contaCelleVive() {
    int celleVive = 0;
    for (int r = 0; r < RIGHE; ++r) {
        for (int c = 0; c < COLONNE; ++c) {
            if (griglia[r][c]) celleVive++;
        }
    }
    return celleVive;
}

void aggiornaSimulazione(Life& life, bool& simulazioneAttiva, GameState& gameState, 
                         sf::Clock& clock, std::vector<Blind>& blinds) {
    if (simulazioneAttiva && clock.getElapsedTime().asMilliseconds() > 150) {
        revive(&life, tipoCella, gameState.denaro);
        gameState.generazioniSopravvissute++;
        clock.restart();
        
        // Controlla se ci sono celle vive
        int celleVive = 0;
        for (int r = 0; r < RIGHE; ++r) {
            for (int c = 0; c < COLONNE; ++c) {
                if (griglia[r][c]) celleVive++;
            }
        }
        
        // Se non ci sono più celle vive, hai perso
        if (celleVive == 0) {
            simulazioneAttiva = false;
            gameState.inGioco = false;
            gameState.blindSuperato = false;
            // Potresti voler azzerare o penalizzare il denaro qui se vuoi
        }
        // Check se il blind è superato
        else if (gameState.generazioniSopravvissute >= blinds[gameState.blindCorrente].obiettivo) {
            gameState.blindSuperato = true;
            gameState.inGioco = false;
            simulazioneAttiva = false;  // Ferma la simulazione
            gameState.denaro += blinds[gameState.blindCorrente].ricompensa;
        }
    }
}

void renderizza(sf::RenderWindow& window, GameState& gameState, Blind& blindCorrente,
                bool simulazioneAttiva, float zoom, sf::Vector2f offset, sf::Font& font) {
    window.clear(sf::Color(20, 20, 25));
    
    disegnaGriglia(window, zoom, offset);
    
    for (int r = 0; r < RIGHE; ++r) {
        for (int c = 0; c < COLONNE; ++c) {
            if (griglia[r][c]) {
                disegnaCella(window, r, c, zoom, offset, tipoCella[r][c]);
            }
        }
    }
    
    disegnaUI(window, gameState, blindCorrente, simulazioneAttiva, font);
    
    window.display();
}

int main() {
    Life life;
    GameState gameState;
    std::vector<Blind> blinds = creaBlindList();
    
    gameState.celleRimanenti = blinds[0].celleDisponibili;
    
    sf::RenderWindow window(sf::VideoMode(LARGHEZZA_FINESTRA, ALTEZZA_FINESTRA), "Conway's Life - Balatro Edition");
    window.setFramerateLimit(60);
    
    // Carica font (usa il font di default del sistema se disponibile)
    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        // Fallback: prova un altro percorso comune
        if (!font.loadFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf")) {
            std::cerr << "Warning: Could not load font, text will not display" << std::endl;
        }
    }
    
    bool isDragging = false;
    bool simulazioneAttiva = false;
    bool modalitaCancellazione = false;
    bool isPanning = false;
    
    float zoom = 1.0f;
    sf::Vector2f offset(0.0f, 0.0f);
    sf::Vector2i lastMousePos;
    
    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            
            gestisciInputMouse(event, isDragging, isPanning, lastMousePos, window, 
                              modalitaCancellazione, zoom, offset, gameState, simulazioneAttiva);
            gestisciPanning(event, isPanning, lastMousePos, offset, window);
            gestisciZoom(event, zoom);
            gestisciInputTastiera(event, simulazioneAttiva, modalitaCancellazione, 
                                 gameState, zoom, offset, clock, blinds);
        }
        
        aggiornaSimulazione(life, simulazioneAttiva, gameState, clock, blinds);
        renderizza(window, gameState, blinds[gameState.blindCorrente], simulazioneAttiva, zoom, offset, font);
    }
    
    return 0;
}