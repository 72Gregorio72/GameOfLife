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
        
        this.gameState = {
            money: 10,
            currentBlind: 0,
            generationsSurvived: 0,
            cellsRemaining: blinds[0].cells,
            selectedCellType: 0,
            inGame: false,
            blindDefeated: false
        };
        
        this.isRunning = false;
        this.isDragging = false;
        this.eraseMode = false;
        this.lastUpdate = 0;
        this.updateInterval = 150;
        
        this.setupEventListeners();
        this.updateUI();
        this.render();
    }
    
    setupEventListeners() {
        this.canvas.addEventListener('mousedown', (e) => this.handleMouseDown(e));
        this.canvas.addEventListener('mousemove', (e) => this.handleMouseMove(e));
        this.canvas.addEventListener('mouseup', () => this.isDragging = false);
        this.canvas.addEventListener('mouseleave', () => this.isDragging = false);
        
        document.addEventListener('keydown', (e) => this.handleKeyPress(e));
        
        document.querySelectorAll('.cell-type-box').forEach(box => {
            box.addEventListener('click', () => {
                this.gameState.selectedCellType = parseInt(box.dataset.type);
                this.updateCellTypeSelection();
            });
        });
    }
    
    handleMouseDown(e) {
        if (this.isRunning) return;
        
        this.isDragging = true;
        this.placeCell(e);
    }
    
    handleMouseMove(e) {
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
        this.updateUI();
        this.gameLoop();
    }
    
    clearGrid() {
        this.grid = Array(ROWS).fill(null).map(() => Array(COLS).fill(false));
        this.cellTypes = Array(ROWS).fill(null).map(() => Array(COLS).fill(0));
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
        
        for (let row = 0; row < ROWS; row++) {
            for (let col = 0; col < COLS; col++) {
                const neighbors = this.countNeighbors(row, col);
                let survives = false;
                let cellType = 0;
                
                if (this.grid[row][col]) {
                    if (neighbors === 2 || neighbors === 3) {
                        const deathProb = deathProbabilities[this.cellTypes[row][col]];
                        if (Math.random() >= deathProb) {
                            survives = true;
                            cellType = this.cellTypes[row][col];
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
    
    gameLoop() {
        if (this.isRunning) {
            const now = Date.now();
            if (now - this.lastUpdate >= this.updateInterval) {
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
                    this.ctx.fillStyle = cellColors[this.cellTypes[row][col]];
                    this.ctx.fillRect(
                        col * CELL_SIZE + 1,
                        row * CELL_SIZE + 1,
                        CELL_SIZE - 2,
                        CELL_SIZE - 2
                    );
                }
            }
        }
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
