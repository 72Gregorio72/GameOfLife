#include "life.hpp"

std::random_device rd;
std::mt19937 gen(rd());

void update_map(Life *life) {
	for (int r = 0; r < RIGHE; ++r) {
		for (int c = 0; c < COLONNE; ++c) {
			life->map[r][c].stato = griglia[r][c];
		}
	}
}

int contaViciniVivi(int r, int c) {
	int count = 0;
	for (int i = -1; i <= 1; ++i) {
		for (int j = -1; j <= 1; ++j) {
			if (i == 0 && j == 0) continue;
			int nr = r + i;
			int nc = c + j;
			if (nr >= 0 && nr < RIGHE && nc >= 0 && nc < COLONNE) {
				if (griglia[nr][nc]) count++;
			}
		}
	}
	return count;
}

float getProbabilitaMorte(int tipo) {
	switch(tipo) {
		case NORMALE: return 0.0f;
		case RISCHIOSA: return 0.10f;
		case MOLTO_RISCHIOSA: return 0.30f;
		case SUPER_RISCHIOSA: return 0.50f;
		default: return 0.0f;
	}
}

float getProbabilitaFatica(int generazioniConsecutive, int blindCorrente) {
	int ante = (blindCorrente / 3) + 1;
	
	float moltiplicatoreFatica = 0.05f + (ante - 1) * 0.01f;
	
	float maxFatica = 0.80f;
	if (ante >= 5) maxFatica = 0.90f;
	else if (ante >= 3) maxFatica = 0.85f;
	
	float fatica = generazioniConsecutive * moltiplicatoreFatica;
	return std::min(fatica, maxFatica);
}

int getValoreCella(int tipo) {
	switch(tipo) {
		case NORMALE: return 1;
		case RISCHIOSA: return 3;
		case MOLTO_RISCHIOSA: return 8;
		case SUPER_RISCHIOSA: return 20;
		default: return 0;
	}
}

sf::Color getColoreCella(int tipo) {
	switch(tipo) {
		case NORMALE: return sf::Color(0, 255, 150);      // Verde
		case RISCHIOSA: return sf::Color(255, 200, 0);     // Giallo
		case MOLTO_RISCHIOSA: return sf::Color(255, 100, 0); // Arancione
		case SUPER_RISCHIOSA: return sf::Color(255, 0, 100);  // Rosso-rosa
		default: return sf::Color::White;
	}
}

void revive(Life *life, std::vector<std::vector<int>>& tipoCella, int& denaro, int blindCorrente) {
	update_map(life);
	std::vector<std::vector<bool>> nuovaGriglia(RIGHE, std::vector<bool>(COLONNE, false));
	std::vector<std::vector<int>> nuovoTipoCella(RIGHE, std::vector<int>(COLONNE, 0));
	std::vector<std::vector<int>> nuoveGenerazioniConsecutive(RIGHE, std::vector<int>(COLONNE, 0));
	
	std::uniform_real_distribution<> dis(0.0, 1.0);
	
	for (int r = 0; r < RIGHE; ++r) {
		for (int c = 0; c < COLONNE; ++c) {
			int viciniVivi = contaViciniVivi(r, c);
			bool sopravvive = false;
			int tipoOrigine = -1;
			
			if (griglia[r][c]) {
				// Cella viva
				if (viciniVivi == 2 || viciniVivi == 3) {
					// Calcola probabilità di morte combinata: tipo + fatica
					float probTipo = getProbabilitaMorte(tipoCella[r][c]);
					float probFatica = getProbabilitaFatica(generazioniConsecutive[r][c], blindCorrente);
					float probTotale = std::min(probTipo + probFatica, 0.95f);  // Max 95%
					
					if (dis(gen) >= probTotale) {
						sopravvive = true;
						tipoOrigine = tipoCella[r][c];
						// Incrementa le generazioni consecutive
						nuoveGenerazioniConsecutive[r][c] = generazioniConsecutive[r][c] + 1;
					}
					// Se muore, generazioniConsecutive rimane 0
				}
			} else {
				// Cella morta
				if (viciniVivi == 3) {
					sopravvive = true;
					// Trova il tipo più comune tra i 3 vicini
					int conteggioTipi[4] = {0, 0, 0, 0};
					for (int i = -1; i <= 1; ++i) {
						for (int j = -1; j <= 1; ++j) {
							if (i == 0 && j == 0) continue;
							int nr = r + i;
							int nc = c + j;
							if (nr >= 0 && nr < RIGHE && nc >= 0 && nc < COLONNE) {
								if (griglia[nr][nc]) {
									conteggioTipi[tipoCella[nr][nc]]++;
								}
							}
						}
					}
					// Trova il tipo più frequente
					int tipoMax = 0;
					int maxConteggio = conteggioTipi[0];
					for (int t = 1; t < 4; ++t) {
						if (conteggioTipi[t] > maxConteggio) {
							maxConteggio = conteggioTipi[t];
							tipoMax = t;
						}
					}
					tipoOrigine = tipoMax;
					// Nuova cella parte da 0 generazioni consecutive
					nuoveGenerazioniConsecutive[r][c] = 0;
				}
			}
			
			if (sopravvive) {
				nuovaGriglia[r][c] = true;
				nuovoTipoCella[r][c] = tipoOrigine;
			}
		}
	}
	
	griglia = nuovaGriglia;
	tipoCella = nuovoTipoCella;
	generazioniConsecutive = nuoveGenerazioniConsecutive;
	
	// Conta il denaro solo per le celle VIVE dopo l'aggiornamento
	for (int r = 0; r < RIGHE; ++r) {
		for (int c = 0; c < COLONNE; ++c) {
			if (griglia[r][c]) {
				denaro += getValoreCella(tipoCella[r][c]);
			}
		}
	}
}