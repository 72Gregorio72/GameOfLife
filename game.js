const CANVAS_WIDTH = 1200;
const CANVAS_HEIGHT = 720;
const CELL_SIZE = 10;
const COLS = Math.floor(CANVAS_WIDTH / CELL_SIZE);
const ROWS = Math.floor(CANVAS_HEIGHT / CELL_SIZE);

const CellType = {
    NORMAL: 0,
    RISKY: 1,
    VERY_RISKY: 2,
    SUPER_RISKY: 3
};

const cellColors = {
    0: '#00ff96',
    1: '#ffc800',
    2: '#ff6400',
    3: '#ff0064'
};

const cellValues = {
    0: 1,
    1: 3,
    2: 8,
    3: 20
};

const deathProbabilities = {
    0: 0.0,
    1: 0.1,
    2: 0.3,
    3: 0.5
};

const blinds = [
    { name: "Small Blind", objective: 5, cells: 15, reward: 50 },
    { name: "Big Blind", objective: 8, cells: 20, reward: 100 },
    { name: "Boss: Oscillator", objective: 12, cells: 25, reward: 200 },
    { name: "Small Blind", objective: 10, cells: 20, reward: 150 },
    { name: "Big Blind", objective: 15, cells: 25, reward: 250 },
    { name: "Boss: Glider", objective: 20, cells: 30, reward: 400 },
    { name: "Small Blind", objective: 18, cells: 25, reward: 300 },
    { name: "Big Blind", objective: 25, cells: 30, reward: 500 },
    { name: "Boss: Eternal", objective: 35, cells: 35, reward: 800 }
];

class Game {
    constructor() {
        this.canvas = document.getElementById('game-canvas');
        this.ctx = this.canvas.getContext('2d');
        this.canvas.width = CANVAS_WIDTH;
        this.canvas.height = CANVAS_HEIGHT;
        
        this.grid = Array(ROWS).fill(null).map(() => Array(COLS).fill(false));
        this.cellTypes = Array(ROWS).fill(null).map(() => Array(COLS).fill(0));
        this.consecutiveGenerations = Array(ROWS).fill(null).map(() => Array(COLS).fill(0));
        
        this.gameState = {
            money: 10,
            currentBlind: 0,
            generationsSurvived: 0,
            cellsRemaining: blinds[0].cells,
            selectedCellType: 0,
            simulationSpeed: 5,  // 1 = slow, 10 = fast
            inGame: false,
            blindDefeated: false
        };
        
        this.isRunning = false;
        this.isDragging = false;
        this.eraseMode = false;
        this.lastUpdate = 0;
        this.isSliderDragging = false;
        
        this.mousePos = { x: -1, y: -1 };
        this.setupEventListeners();
        this.updateUI();
        this.render();
    }
    
    setupEventListeners() {
        this.canvas.addEventListener('mousedown', (e) => this.handleMouseDown(e));
        this.canvas.addEventListener('mousemove', (e) => this.handleMouseMove(e));
        this.canvas.addEventListener('mouseup', () => {
            this.isDragging = false;
            this.isSliderDragging = false;
        });
        this.canvas.addEventListener('mouseleave', () => {
            this.isDragging = false;
            this.isSliderDragging = false;
        });
        
        document.addEventListener('keydown', (e) => this.handleKeyPress(e));
        
        document.querySelectorAll('.cell-type-box').forEach(box => {
            box.addEventListener('click', () => {
                this.gameState.selectedCellType = parseInt(box.dataset.type);
                this.updateCellTypeSelection();
            });
        });
    }
    
    handleSliderClick(e) {
        const rect = this.canvas.getBoundingClientRect();
        const x = e.clientX - rect.left;
        const y = e.clientY - rect.top;
        
        // Slider bounds (matching C++ position)
        const sliderX = CANVAS_WIDTH - 220;
        const sliderY = 15;
        const sliderWidth = 150;
        const sliderHeight = 8;
        
        if (x >= sliderX && x <= sliderX + sliderWidth &&
            y >= sliderY - 5 && y <= sliderY + sliderHeight + 5) {
            const relativeX = x - sliderX;
            this.gameState.simulationSpeed = Math.max(1, Math.min(10, Math.floor((relativeX / sliderWidth) * 10) + 1));
            this.isSliderDragging = true;
            return true;
        }
        return false;
    }
    
    handleMouseDown(e) {
        // Check slider first
        if (this.handleSliderClick(e)) {
            return;
        }
        
        if (this.isRunning) return;
        
        this.isDragging = true;
        this.placeCell(e);
    }
    
    handleMouseMove(e) {
        this.updateMousePos(e);
        
        if (this.isSliderDragging) {
            this.handleSliderClick(e);
            return;
        }
        
        if (this.isDragging && !this.isRunning) {
            this.placeCell(e);
        }
    }
    
    placeCell(e) {
        const rect = this.canvas.getBoundingClientRect();
        const x = e.clientX - rect.left;
        const y = e.clientY - rect.top;
        
        const col = Math.floor(x / CELL_SIZE);
        const row = Math.floor(y / CELL_SIZE);
        
        if (row >= 0 && row < ROWS && col >= 0 && col < COLS) {
            if (this.eraseMode) {
                if (this.grid[row][col]) {
                    this.grid[row][col] = false;
                    this.cellTypes[row][col] = 0;
                    this.gameState.cellsRemaining++;
                }
            } else {
                if (!this.grid[row][col] && this.gameState.cellsRemaining > 0) {
                    this.grid[row][col] = true;
                    this.cellTypes[row][col] = this.gameState.selectedCellType;
                    this.gameState.cellsRemaining--;
                }
            }
            this.updateUI();
            this.render();
        }
    }
    
    handleKeyPress(e) {
        switch(e.key.toLowerCase()) {
            case ' ':
                if (!this.gameState.inGame && this.gameState.cellsRemaining === 0) {
                    this.startGame();
                } else if (this.gameState.inGame) {
                    this.isRunning = !this.isRunning;
                    this.updateUI();
                }
                break;
            case 'e':
                this.eraseMode = !this.eraseMode;
                break;
            case 'c':
                this.clearGrid();
                break;
            case 'n':
                if (!this.gameState.inGame && this.gameState.blindDefeated) {
                    this.nextBlind();
                }
                break;
            case '1':
                this.gameState.selectedCellType = 0;
                this.updateCellTypeSelection();
                break;
            case '2':
                this.gameState.selectedCellType = 1;
                this.updateCellTypeSelection();
                break;
            case '3':
                this.gameState.selectedCellType = 2;
                this.updateCellTypeSelection();
                break;
            case '4':
                this.gameState.selectedCellType = 3;
                this.updateCellTypeSelection();
                break;
        }
    }
    
    startGame() {
        this.gameState.inGame = true;
        this.gameState.generationsSurvived = 0;
        this.isRunning = true;
        this.lastUpdate = Date.now();
        
        // Reset consecutive generations
        this.consecutiveGenerations = Array(ROWS).fill(null).map(() => Array(COLS).fill(0));
        
        this.updateUI();
        this.gameLoop();
    }
    
    clearGrid() {
        this.grid = Array(ROWS).fill(null).map(() => Array(COLS).fill(false));
        this.cellTypes = Array(ROWS).fill(null).map(() => Array(COLS).fill(0));
        this.consecutiveGenerations = Array(ROWS).fill(null).map(() => Array(COLS).fill(0));
        this.isRunning = false;
        this.gameState.cellsRemaining = blinds[this.gameState.currentBlind].cells;
        this.gameState.inGame = false;
        this.gameState.generationsSurvived = 0;
        this.updateUI();
        this.render();
    }
    
    nextBlind() {
        if (this.gameState.currentBlind < blinds.length - 1) {
            this.gameState.currentBlind++;
            this.gameState.cellsRemaining = blinds[this.gameState.currentBlind].cells;
            this.gameState.generationsSurvived = 0;
            this.gameState.blindDefeated = false;
            this.clearGrid();
        }
    }
    
    hexToRgb(hex) {
        const result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
        return result ? {
            r: parseInt(result[1], 16),
            g: parseInt(result[2], 16),
            b: parseInt(result[3], 16)
        } : { r: 255, g: 255, b: 255 };
    }
    
    getFatigueProbability(consecutiveGens) {
        const ante = Math.floor(this.gameState.currentBlind / 3) + 1;
        const fatigueMultiplier = 0.05 + (ante - 1) * 0.01;
        
        let maxFatigue = 0.80;
        if (ante >= 5) maxFatigue = 0.90;
        else if (ante >= 3) maxFatigue = 0.85;
        
        const fatigue = consecutiveGens * fatigueMultiplier;
        return Math.min(fatigue, maxFatigue);
    }
    
    countNeighbors(row, col) {
        let count = 0;
        for (let i = -1; i <= 1; i++) {
            for (let j = -1; j <= 1; j++) {
                if (i === 0 && j === 0) continue;
                const newRow = row + i;
                const newCol = col + j;
                if (newRow >= 0 && newRow < ROWS && newCol >= 0 && newCol < COLS) {
                    if (this.grid[newRow][newCol]) count++;
                }
            }
        }
        return count;
    }
    
    updateGeneration() {
        const newGrid = Array(ROWS).fill(null).map(() => Array(COLS).fill(false));
        const newCellTypes = Array(ROWS).fill(null).map(() => Array(COLS).fill(0));
        const newConsecutiveGens = Array(ROWS).fill(null).map(() => Array(COLS).fill(0));
        
        for (let row = 0; row < ROWS; row++) {
            for (let col = 0; col < COLS; col++) {
                const neighbors = this.countNeighbors(row, col);
                let survives = false;
                let cellType = 0;
                
                if (this.grid[row][col]) {
                    if (neighbors === 2 || neighbors === 3) {
                        const typeProb = deathProbabilities[this.cellTypes[row][col]];
                        const fatigueProb = this.getFatigueProbability(this.consecutiveGenerations[row][col]);
                        const totalProb = Math.min(typeProb + fatigueProb, 0.95);
                        
                        if (Math.random() >= totalProb) {
                            survives = true;
                            cellType = this.cellTypes[row][col];
                            newConsecutiveGens[row][col] = this.consecutiveGenerations[row][col] + 1;
                        }
                    }
                } else {
                    if (neighbors === 3) {
                        survives = true;
                        const typeCounts = [0, 0, 0, 0];
                        for (let i = -1; i <= 1; i++) {
                            for (let j = -1; j <= 1; j++) {
                                if (i === 0 && j === 0) continue;
                                const newRow = row + i;
                                const newCol = col + j;
                                if (newRow >= 0 && newRow < ROWS && newCol >= 0 && newCol < COLS) {
                                    if (this.grid[newRow][newCol]) {
                                        typeCounts[this.cellTypes[newRow][newCol]]++;
                                    }
                                }
                            }
                        }
                        cellType = typeCounts.indexOf(Math.max(...typeCounts));
                        newConsecutiveGens[row][col] = 0;
                    }
                }
                
                if (survives) {
                    newGrid[row][col] = true;
                    newCellTypes[row][col] = cellType;
                }
            }
        }
        
        this.grid = newGrid;
        this.cellTypes = newCellTypes;
        this.consecutiveGenerations = newConsecutiveGens;
        
        let aliveCells = 0;
        for (let row = 0; row < ROWS; row++) {
            for (let col = 0; col < COLS; col++) {
                if (this.grid[row][col]) {
                    aliveCells++;
                    this.gameState.money += cellValues[this.cellTypes[row][col]];
                }
            }
        }
        
        this.gameState.generationsSurvived++;
        
        if (aliveCells === 0) {
            this.isRunning = false;
            this.gameState.inGame = false;
            this.gameState.blindDefeated = false;
        } else if (this.gameState.generationsSurvived >= blinds[this.gameState.currentBlind].objective) {
            this.gameState.blindDefeated = true;
            this.gameState.inGame = false;
            this.isRunning = false;
            this.gameState.money += blinds[this.gameState.currentBlind].reward;
        }
        
        this.updateUI();
    }
    
    getUpdateInterval() {
        // Speed 1 = 300ms, Speed 10 = 30ms
        return 330 - (this.gameState.simulationSpeed * 30);
    }
    
    gameLoop() {
        if (this.isRunning) {
            const now = Date.now();
            if (now - this.lastUpdate >= this.getUpdateInterval()) {
                this.updateGeneration();
                this.lastUpdate = now;
            }
            requestAnimationFrame(() => this.gameLoop());
        }
        this.render();
    }
    
    render() {
        this.ctx.fillStyle = '#14141a';
        this.ctx.fillRect(0, 0, CANVAS_WIDTH, CANVAS_HEIGHT);
        
        this.ctx.strokeStyle = '#282828';
        for (let i = 0; i <= COLS; i++) {
            this.ctx.beginPath();
            this.ctx.moveTo(i * CELL_SIZE, 0);
            this.ctx.lineTo(i * CELL_SIZE, CANVAS_HEIGHT);
            this.ctx.stroke();
        }
        for (let i = 0; i <= ROWS; i++) {
            this.ctx.beginPath();
            this.ctx.moveTo(0, i * CELL_SIZE);
            this.ctx.lineTo(CANVAS_WIDTH, i * CELL_SIZE);
            this.ctx.stroke();
        }
        
        for (let row = 0; row < ROWS; row++) {
            for (let col = 0; col < COLS; col++) {
                if (this.grid[row][col]) {
                    // Calculate total risk
                    const typeProb = deathProbabilities[this.cellTypes[row][col]];
                    const fatigueProb = this.getFatigueProbability(this.consecutiveGenerations[row][col]);
                    const totalProb = Math.min(typeProb + fatigueProb, 0.95);
                    
                    // Gradient: green -> yellow -> orange -> red
                    let finalColor;
                    
                    if (totalProb < 0.33) {
                        // Green -> Yellow (0% - 33%)
                        const t = totalProb / 0.33;
                        finalColor = {
                            r: Math.round(0 + 255 * t),      // R: 0 -> 255
                            g: 255,                           // G: 255 (fixed)
                            b: Math.round(150 * (1 - t))     // B: 150 -> 0
                        };
                    } else if (totalProb < 0.66) {
                        // Yellow -> Orange (33% - 66%)
                        const t = (totalProb - 0.33) / 0.33;
                        finalColor = {
                            r: 255,                           // R: 255 (fixed)
                            g: Math.round(255 * (1 - t * 0.6)), // G: 255 -> 100
                            b: 0                              // B: 0 (fixed)
                        };
                    } else {
                        // Orange -> Red (66% - 100%)
                        const t = (totalProb - 0.66) / 0.34;
                        finalColor = {
                            r: 255,                           // R: 255 (fixed)
                            g: Math.round(100 * (1 - t)),    // G: 100 -> 0
                            b: 0                              // B: 0 (fixed)
                        };
                    }
                    
                    // Draw cell with gradient color
                    this.ctx.fillStyle = `rgb(${finalColor.r}, ${finalColor.g}, ${finalColor.b})`;
                    this.ctx.fillRect(
                        col * CELL_SIZE + 1,
                        row * CELL_SIZE + 1,
                        CELL_SIZE - 2,
                        CELL_SIZE - 2
                    );
                    
                    // Draw warning border for very high risk cells (>75%)
                    if (totalProb > 0.75) {
                        this.ctx.strokeStyle = '#ffffff'; // White
                        this.ctx.lineWidth = Math.min((totalProb - 0.75) * 10, 2.5);
                        this.ctx.strokeRect(
                            col * CELL_SIZE + 1,
                            row * CELL_SIZE + 1,
                            CELL_SIZE - 2,
                            CELL_SIZE - 2
                        );
                        this.ctx.lineWidth = 1;
                    }
                }
            }
        }
        
        // Draw tooltip on hover
        if (this.gameState.inGame && this.mousePos.x >= 0) {
            const col = Math.floor(this.mousePos.x / CELL_SIZE);
            const row = Math.floor(this.mousePos.y / CELL_SIZE);
            
            if (row >= 0 && row < ROWS && col >= 0 && col < COLS && this.grid[row][col]) {
                this.drawCellTooltip(row, col);
            }
        }
        
        // Draw speed slider (top right)
        const sliderX = CANVAS_WIDTH - 220;
        const sliderY = 15;
        const sliderWidth = 150;
        const sliderHeight = 8;
        
        // Label
        this.ctx.fillStyle = '#ffffff';
        this.ctx.font = '12px Arial';
        this.ctx.fillText('Speed', sliderX, sliderY - 5);
        
        // Background
        this.ctx.fillStyle = '#323232';
        this.ctx.fillRect(sliderX, sliderY, sliderWidth, sliderHeight);
        this.ctx.strokeStyle = '#666666';
        this.ctx.strokeRect(sliderX, sliderY, sliderWidth, sliderHeight);
        
        // Fill (current speed)
        const fillWidth = (sliderWidth * this.gameState.simulationSpeed) / 10;
        let speedColor;
        if (this.gameState.simulationSpeed <= 3) speedColor = '#6496ff';
        else if (this.gameState.simulationSpeed <= 7) speedColor = '#00ff96';
        else speedColor = '#ff6464';
        
        this.ctx.fillStyle = speedColor;
        this.ctx.fillRect(sliderX, sliderY, fillWidth, sliderHeight);
        
        // Knob
        const knobX = sliderX + (sliderWidth * this.gameState.simulationSpeed) / 10 - 6;
        this.ctx.fillStyle = '#ffffff';
        this.ctx.beginPath();
        this.ctx.arc(knobX + 6, sliderY + 4, 6, 0, Math.PI * 2);
        this.ctx.fill();
        this.ctx.strokeStyle = '#cccccc';
        this.ctx.stroke();
        
        // Value
        this.ctx.fillStyle = '#cccccc';
        this.ctx.font = '10px Arial';
        this.ctx.fillText(this.gameState.simulationSpeed + 'x', sliderX + sliderWidth + 10, sliderY + 7);
    }
    
    drawCellTooltip(row, col) {
        const x = this.mousePos.x + 15;
        const y = this.mousePos.y + 15;
        
        // Background
        this.ctx.fillStyle = 'rgba(20, 20, 25, 0.95)';
        this.ctx.fillRect(x, y, 180, 100);
        this.ctx.strokeStyle = '#ffd700';
        this.ctx.lineWidth = 2;
        this.ctx.strokeRect(x, y, 180, 100);
        this.ctx.lineWidth = 1;
        
        // Calculate probabilities
        const typeProb = deathProbabilities[this.cellTypes[row][col]];
        const fatigueProb = this.getFatigueProbability(this.consecutiveGenerations[row][col]);
        const totalProb = Math.min(typeProb + fatigueProb, 0.95);
        
        // Text
        this.ctx.fillStyle = '#ffffff';
        this.ctx.font = '11px Arial';
        const typeNames = ["Normal", "Risky", "Very Risky", "Super Risky"];
        
        this.ctx.fillText(`Type: ${typeNames[this.cellTypes[row][col]]}`, x + 10, y + 20);
        this.ctx.fillText(`Generations: ${this.consecutiveGenerations[row][col]}`, x + 10, y + 35);
        this.ctx.fillText(`Type risk: ${Math.round(typeProb * 100)}%`, x + 10, y + 50);
        this.ctx.fillText(`Fatigue: ${Math.round(fatigueProb * 100)}%`, x + 10, y + 65);
        this.ctx.fillText(`TOTAL RISK: ${Math.round(totalProb * 100)}%`, x + 10, y + 80);
        
        // Risk bar
        const barWidth = 160 * totalProb;
        let barColor;
        if (totalProb < 0.3) barColor = '#00ff00';
        else if (totalProb < 0.6) barColor = '#ffc800';
        else barColor = '#ff0000';
        
        this.ctx.fillStyle = barColor;
        this.ctx.fillRect(x + 10, y + 85, barWidth, 8);
        
        this.ctx.strokeStyle = '#666666';
        this.ctx.strokeRect(x + 10, y + 85, 160, 8);
    }
    
    updateUI() {
        document.getElementById('money-text').textContent = '$' + this.gameState.money;
        document.getElementById('blind-name').textContent = blinds[this.gameState.currentBlind].name;
        
        const progress = Math.min(this.gameState.generationsSurvived / blinds[this.gameState.currentBlind].objective, 1) * 100;
        document.getElementById('progress-bar').style.width = progress + '%';
        document.getElementById('progress-text').textContent = 
            this.gameState.generationsSurvived + '/' + blinds[this.gameState.currentBlind].objective;
        
        document.getElementById('reward-text').textContent = 'Prize: $' + blinds[this.gameState.currentBlind].reward;
        document.getElementById('cells-text').textContent = 'Cells: ' + this.gameState.cellsRemaining;
        
        const statusIndicator = document.getElementById('status-indicator');
        const statusText = document.getElementById('status-text');
        statusIndicator.className = 'status-indicator';
        if (this.isRunning) {
            statusIndicator.classList.add('running');
            statusText.textContent = 'Running';
        } else if (this.gameState.inGame) {
            statusIndicator.classList.add('paused');
            statusText.textContent = 'Paused';
        } else {
            statusText.textContent = 'Setup';
        }
        
        const victoryMessage = document.getElementById('victory-message');
        if (this.gameState.blindDefeated && !this.gameState.inGame) {
            victoryMessage.classList.add('show');
        } else {
            victoryMessage.classList.remove('show');
        }
        
        this.updateCellTypeSelection();
    }
    
    updateCellTypeSelection() {
        document.querySelectorAll('.cell-type-box').forEach(box => {
            if (parseInt(box.dataset.type) === this.gameState.selectedCellType) {
                box.classList.add('selected');
            } else {
                box.classList.remove('selected');
            }
        });
    }
}

window.addEventListener('load', () => {
    new Game();
});
