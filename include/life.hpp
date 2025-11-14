#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <sstream>
#include <random>
#include <ctime>

const int LARGHEZZA_FINESTRA = 1200;
const int ALTEZZA_FINESTRA = 800;
const int DIMENSIONE_CELLA = 10;
const int ALTEZZA_UI = 80;

const int RIGHE = (ALTEZZA_FINESTRA - ALTEZZA_UI) / DIMENSIONE_CELLA;
const int COLONNE = LARGHEZZA_FINESTRA / DIMENSIONE_CELLA;
extern std::vector<std::vector<bool>> griglia;
extern std::vector<std::vector<int>> tipoCella;
extern std::vector<std::vector<int>> generazioniConsecutive;

enum TipoCella {
    NORMALE = 0,
    RISCHIOSA = 1,
    MOLTO_RISCHIOSA = 2,
    SUPER_RISCHIOSA = 3
};

struct Blind {
    std::string nome;
    int obiettivo;
    int celleDisponibili;
    int ricompensa;
    
    Blind(std::string n, int obj, int celle, int ric) 
        : nome(n), obiettivo(obj), celleDisponibili(celle), ricompensa(ric) {}
};

class Cell {
	public:
		bool stato;

		Cell() : stato(false) {}
};

struct Life {
	std::vector<std::vector<Cell>> map;
	
	Life() : map(RIGHE, std::vector<Cell>(COLONNE)) {}
};

struct GameState {
    int denaro;
    int blindCorrente;
    int generazioniSopravvissute;
    int celleRimanenti;
    int tipoCellaSelezionata;
    int velocitaSimulazione;  // 1 = molto lento, 10 = molto veloce
    bool inGioco;
    bool blindSuperato;
    
    GameState() : denaro(10), blindCorrente(0), generazioniSopravvissute(0), 
                  celleRimanenti(0), tipoCellaSelezionata(0), velocitaSimulazione(5),
                  inGioco(false), blindSuperato(false) {}
};

void update_map(Life *life);
void revive(Life *life, std::vector<std::vector<int>>& tipoCella, int& denaro, int blindCorrente);
int contaViciniVivi(int r, int c);
float getProbabilitaMorte(int tipo);
float getProbabilitaFatica(int generazioniConsecutive, int blindCorrente);
int getValoreCella(int tipo);
sf::Color getColoreCella(int tipo);