#include "life.hpp"

std::vector<Blind> creaBlindList() {
    std::vector<Blind> blinds;
    
    // Small Blind
    blinds.push_back(Blind("Small Blind", 5, 15, 50));
    blinds.push_back(Blind("Big Blind", 8, 20, 100));
    blinds.push_back(Blind("Boss: Oscillator", 12, 25, 200));
    
    // Ante 2
    blinds.push_back(Blind("Small Blind", 10, 20, 150));
    blinds.push_back(Blind("Big Blind", 15, 25, 250));
    blinds.push_back(Blind("Boss: Glider", 20, 30, 400));
    
    // Ante 3
    blinds.push_back(Blind("Small Blind", 18, 25, 300));
    blinds.push_back(Blind("Big Blind", 25, 30, 500));
    blinds.push_back(Blind("Boss: Eternal", 35, 35, 800));
    
    return blinds;
}
