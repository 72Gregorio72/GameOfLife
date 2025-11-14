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

	// Ante 4
	blinds.push_back(Blind("Small Blind", 30, 30, 600));
	blinds.push_back(Blind("Big Blind", 40, 35, 1000));
	blinds.push_back(Blind("Boss: Pulsar", 50, 45, 1600));

	// Ante 5
	blinds.push_back(Blind("Small Blind", 45, 35, 900));
	blinds.push_back(Blind("Big Blind", 55, 40, 1400));
	blinds.push_back(Blind("Boss: Beacon", 70, 55, 2200));

	// Ante 6
	blinds.push_back(Blind("Small Blind", 65, 45, 1300));
	blinds.push_back(Blind("Big Blind", 80, 55, 2000));
	blinds.push_back(Blind("Boss: Glider Gun", 100, 70, 3200));

	// Ante 7
	blinds.push_back(Blind("Small Blind", 90, 60, 1800));
	blinds.push_back(Blind("Big Blind", 115, 80, 2800));
	blinds.push_back(Blind("Boss: Lightweight Spaceship", 150, 100, 4500));

	// Ante 8
	blinds.push_back(Blind("Small Blind", 130, 85, 2600));
	blinds.push_back(Blind("Big Blind", 170, 120, 4100));
	blinds.push_back(Blind("Boss: Diehard", 220, 150, 6500));

	// Ante 9
	blinds.push_back(Blind("Small Blind", 190, 130, 3800));
	blinds.push_back(Blind("Big Blind", 250, 180, 6000));
	blinds.push_back(Blind("Boss: Caterpillar", 320, 240, 9500));

	// Ante 10
	blinds.push_back(Blind("Small Blind", 280, 190, 5600));
	blinds.push_back(Blind("Big Blind", 370, 260, 9000));
	blinds.push_back(Blind("Boss: Ultimate", 480, 360, 14000));
    
    return blinds;
}
