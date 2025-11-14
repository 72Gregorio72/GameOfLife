#include "life.hpp"

std::vector<std::vector<bool>> griglia(RIGHE, std::vector<bool>(COLONNE, false));
std::vector<std::vector<int>> tipoCella(RIGHE, std::vector<int>(COLONNE, 0));
std::vector<std::vector<int>> generazioniConsecutive(RIGHE, std::vector<int>(COLONNE, 0));

extern std::vector<Blind> creaBlindList();

void disegnaCella(sf::RenderWindow& window, int riga, int colonna, float zoom, sf::Vector2f offset, int tipo, int blindCorrente) {
    float dimensioneCellaZoom = DIMENSIONE_CELLA * zoom;
    sf::RectangleShape cella(sf::Vector2f(dimensioneCellaZoom - 1, dimensioneCellaZoom - 1));
    cella.setPosition(colonna * dimensioneCellaZoom + offset.x, riga * dimensioneCellaZoom + offset.y + ALTEZZA_UI);
    
    // Calcola probabilità totale di morte (tipo + fatica)
    float probTipo = getProbabilitaMorte(tipo);
    float probFatica = getProbabilitaFatica(generazioniConsecutive[riga][colonna], blindCorrente);
    float probTotale = std::min(probTipo + probFatica, 0.95f);
    
    // Gradazione verde -> giallo -> arancione -> rosso
    sf::Color coloreFinal;
    
    if (probTotale < 0.33f) {
        // Verde -> Giallo (0% - 33%)
        float t = probTotale / 0.33f;
        coloreFinal = sf::Color(
            static_cast<sf::Uint8>(0 + 255 * t),      // R: 0 -> 255
            255,                                        // G: 255 (fisso)
            static_cast<sf::Uint8>(150 * (1 - t))     // B: 150 -> 0
        );
    } else if (probTotale < 0.66f) {
        // Giallo -> Arancione (33% - 66%)
        float t = (probTotale - 0.33f) / 0.33f;
        coloreFinal = sf::Color(
            255,                                        // R: 255 (fisso)
            static_cast<sf::Uint8>(255 * (1 - t * 0.6f)), // G: 255 -> 100
            0                                           // B: 0 (fisso)
        );
    } else {
        // Arancione -> Rosso (66% - 100%)
        float t = (probTotale - 0.66f) / 0.34f;
        coloreFinal = sf::Color(
            255,                                        // R: 255 (fisso)
            static_cast<sf::Uint8>(100 * (1 - t)),     // G: 100 -> 0
            0                                           // B: 0 (fisso)
        );
    }
    
    cella.setFillColor(coloreFinal);
    window.draw(cella);
    
    // Bordo di pericolo per celle molto a rischio (>75%)
    if (probTotale > 0.75f) {
        sf::RectangleShape bordoPericolo(sf::Vector2f(dimensioneCellaZoom - 1, dimensioneCellaZoom - 1));
        bordoPericolo.setPosition(colonna * dimensioneCellaZoom + offset.x, riga * dimensioneCellaZoom + offset.y + ALTEZZA_UI);
        bordoPericolo.setFillColor(sf::Color::Transparent);
        bordoPericolo.setOutlineColor(sf::Color(255, 255, 255)); // Bianco per massimo pericolo
        bordoPericolo.setOutlineThickness(std::min((probTotale - 0.75f) * 10.0f, 2.5f));
        window.draw(bordoPericolo);
    }
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
    
    // Slider velocità (in alto a destra)
    float sliderX = LARGHEZZA_FINESTRA - 220;
    float sliderY = 15;
    float sliderWidth = 150;
    float sliderHeight = 8;
    
    // Label velocità
    sf::Text labelVelocita;
    labelVelocita.setFont(font);
    labelVelocita.setString("Speed");
    labelVelocita.setCharacterSize(12);
    labelVelocita.setFillColor(sf::Color::White);
    labelVelocita.setPosition(sliderX, sliderY - 15);
    window.draw(labelVelocita);
    
    // Sfondo slider
    sf::RectangleShape sliderBg(sf::Vector2f(sliderWidth, sliderHeight));
    sliderBg.setPosition(sliderX, sliderY);
    sliderBg.setFillColor(sf::Color(50, 50, 50));
    sliderBg.setOutlineColor(sf::Color(100, 100, 100));
    sliderBg.setOutlineThickness(1);
    window.draw(sliderBg);
    
    // Riempimento slider (indica velocità corrente)
    float fillWidth = (sliderWidth * gameState.velocitaSimulazione) / 10.0f;
    sf::RectangleShape sliderFill(sf::Vector2f(fillWidth, sliderHeight));
    sliderFill.setPosition(sliderX, sliderY);
    sf::Color colorVelocita;
    if (gameState.velocitaSimulazione <= 3) colorVelocita = sf::Color(100, 150, 255); // Blu (lento)
    else if (gameState.velocitaSimulazione <= 7) colorVelocita = sf::Color(0, 255, 150); // Verde (medio)
    else colorVelocita = sf::Color(255, 100, 100); // Rosso (veloce)
    sliderFill.setFillColor(colorVelocita);
    window.draw(sliderFill);
    
    // Indicatore posizione (pallino)
    float knobX = sliderX + (sliderWidth * gameState.velocitaSimulazione) / 10.0f - 6;
    sf::CircleShape knob(6);
    knob.setPosition(knobX, sliderY - 3);
    knob.setFillColor(sf::Color::White);
    knob.setOutlineColor(sf::Color(200, 200, 200));
    knob.setOutlineThickness(1);
    window.draw(knob);
    
    // Valore numerico
    sf::Text valorVelocita;
    valorVelocita.setFont(font);
    valorVelocita.setString(std::to_string(gameState.velocitaSimulazione) + "x");
    valorVelocita.setCharacterSize(10);
    valorVelocita.setFillColor(sf::Color(200, 200, 200));
    valorVelocita.setPosition(sliderX + sliderWidth + 10, sliderY - 2);
    window.draw(valorVelocita);
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

void gestisciSliderVelocita(int mouseX, int mouseY, GameState& gameState) {
    float sliderX = LARGHEZZA_FINESTRA - 220;
    float sliderY = 15;
    float sliderWidth = 150;
    float sliderHeight = 8;
    
    // Verifica se il click è dentro lo slider
    if (mouseX >= sliderX && mouseX <= sliderX + sliderWidth &&
        mouseY >= sliderY - 5 && mouseY <= sliderY + sliderHeight + 5) {
        
        // Calcola nuova velocità in base alla posizione del click
        float relativeX = mouseX - sliderX;
        int nuovaVelocita = std::max(1, std::min(10, (int)((relativeX / sliderWidth) * 10.0f) + 1));
        gameState.velocitaSimulazione = nuovaVelocita;
    }
}

void gestisciInputMouse(sf::Event& event, bool& isDragging, bool& isPanning, 
                        sf::Vector2i& lastMousePos, sf::RenderWindow& window,
                        bool modalitaCancellazione, float zoom, sf::Vector2f offset, GameState& gameState, bool simulazioneAttiva) {
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            // Check if clicking on speed slider (in UI area)
            if (event.mouseButton.y < ALTEZZA_UI) {
                gestisciSliderVelocita(event.mouseButton.x, event.mouseButton.y, gameState);
                return;
            }
            
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
    
    // Permetti di trascinare lo slider anche durante il movimento
    if (event.type == sf::Event::MouseMoved) {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            if (mousePos.y < ALTEZZA_UI) {
                gestisciSliderVelocita(mousePos.x, mousePos.y, gameState);
            }
        }
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
                // Reset generazioni consecutive all'inizio
                for (int r = 0; r < RIGHE; ++r) {
                    for (int c = 0; c < COLONNE; ++c) {
                        generazioniConsecutive[r][c] = 0;
                    }
                }
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
                    generazioniConsecutive[r][c] = 0;
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
                        generazioniConsecutive[r][c] = 0;
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
    // Calcola intervallo basato sulla velocità: velocità 1 = 300ms, velocità 10 = 30ms
    int intervallo = 330 - (gameState.velocitaSimulazione * 30);
    
    if (simulazioneAttiva && clock.getElapsedTime().asMilliseconds() > intervallo) {
        revive(&life, tipoCella, gameState.denaro, gameState.blindCorrente);
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
        }
        // Check se il blind è superato
        else if (gameState.generazioniSopravvissute >= blinds[gameState.blindCorrente].obiettivo) {
            gameState.blindSuperato = true;
            gameState.inGioco = false;
            simulazioneAttiva = false;
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
                disegnaCella(window, r, c, zoom, offset, tipoCella[r][c], gameState.blindCorrente);
            }
        }
    }
    
    // Mostra info cella sotto il mouse
    if (gameState.inGioco) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        if (mousePos.y > ALTEZZA_UI) {
            float dimensioneCellaZoom = DIMENSIONE_CELLA * zoom;
            int col = (mousePos.x - offset.x) / dimensioneCellaZoom;
            int row = (mousePos.y - ALTEZZA_UI - offset.y) / dimensioneCellaZoom;
            
            if (row >= 0 && row < RIGHE && col >= 0 && col < COLONNE && griglia[row][col]) {
                // Box info
                sf::RectangleShape infoBox(sf::Vector2f(180, 70));
                infoBox.setPosition(mousePos.x + 15, mousePos.y + 15);
                infoBox.setFillColor(sf::Color(20, 20, 25, 230));
                infoBox.setOutlineColor(sf::Color(255, 215, 0));
                infoBox.setOutlineThickness(2);
                window.draw(infoBox);
                
                // Testo info
                sf::Text infoText;
                infoText.setFont(font);
                infoText.setCharacterSize(11);
                infoText.setFillColor(sf::Color::White);
                
                float probTipo = getProbabilitaMorte(tipoCella[row][col]);
                float probFatica = getProbabilitaFatica(generazioniConsecutive[row][col], gameState.blindCorrente);
                float probTotale = std::min(probTipo + probFatica, 0.95f);
                
                int ante = (gameState.blindCorrente / 3) + 1;
                
                std::string nomiTipi[4] = {"Normal", "Risky", "Very Risky", "Super Risky"};
                std::stringstream ss;
                ss << "Type: " << nomiTipi[tipoCella[row][col]] << "\n";
                ss << "Generations: " << generazioniConsecutive[row][col] << "\n";
                ss << "Ante: " << ante << " (+" << (int)((0.05f + (ante - 1) * 0.01f) * 100) << "%/gen)\n";
                ss << "Type risk: " << (int)(probTipo * 100) << "%\n";
                ss << "Fatigue: " << (int)(probFatica * 100) << "%\n";
                ss << "TOTAL RISK: " << (int)(probTotale * 100) << "%";
                
                infoText.setString(ss.str());
                infoText.setPosition(mousePos.x + 20, mousePos.y + 20);
                
                window.draw(infoText);
                
                // Barra di pericolo visuale
                sf::RectangleShape barraRischio(sf::Vector2f(160 * probTotale, 8));
                barraRischio.setPosition(mousePos.x + 25, mousePos.y + 75);
                sf::Color coloreRischio;
                if (probTotale < 0.3f) coloreRischio = sf::Color(0, 255, 0);
                else if (probTotale < 0.6f) coloreRischio = sf::Color(255, 200, 0);
                else coloreRischio = sf::Color(255, 0, 0);
                barraRischio.setFillColor(coloreRischio);
                window.draw(barraRischio);
                
                sf::RectangleShape bordoBarra(sf::Vector2f(160, 8));
                bordoBarra.setPosition(mousePos.x + 25, mousePos.y + 75);
                bordoBarra.setFillColor(sf::Color::Transparent);
                bordoBarra.setOutlineColor(sf::Color(100, 100, 100));
                bordoBarra.setOutlineThickness(1);
                window.draw(bordoBarra);
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
    
    // Carica font - prova multipli percorsi per compatibilità cross-platform
    sf::Font font;
    bool fontLoaded = false;
    
    // Lista di percorsi da provare
    std::vector<std::string> fontPaths = {
        // Windows
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/Arial.ttf",
        "arial.ttf",  // Copia locale
        "font.ttf",   // Font incluso nel progetto
        // Linux
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf"
    };
    
    for (const auto& path : fontPaths) {
        if (font.loadFromFile(path)) {
            fontLoaded = true;
            std::cout << "Font loaded from: " << path << std::endl;
            break;
        }
    }
    
    if (!fontLoaded) {
        std::cerr << "ERROR: Could not load any font!" << std::endl;
        std::cerr << "Please copy a TrueType font (.ttf) to the game directory and name it 'font.ttf'" << std::endl;
        // Continua comunque - il gioco funzionerà ma senza testo
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