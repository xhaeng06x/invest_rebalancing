// Main State
let assets = {
    stocks: [],
    bonds: [],
    gold: 0,
    total: 0,
    totalStock: 0,
    totalBond: 0
};

// Initialization
document.addEventListener('DOMContentLoaded', () => {
    loadData();
    if (assets.stocks.length === 0) addStockInput();
    if (assets.bonds.length === 0) addBondInput();
});

// UI Helpers
function formatCurrency(amount) {
    return new Intl.NumberFormat('ko-KR').format(Math.round(amount)) + '원';
}

function addStockInput(name = '', price = '') {
    const list = document.getElementById('stock-list');
    const div = document.createElement('div');
    div.className = 'input-item';
    div.innerHTML = `
        <input type="text" placeholder="종목명" class="stock-name" value="${name}" style="flex: 1;">
        <input type="number" placeholder="평가금액" class="stock-price" value="${price}" style="flex: 1;">
        <button class="btn-danger" style="width: auto; padding: 0 12px; margin: 0;" onclick="this.parentElement.remove()">x</button>
    `;
    list.appendChild(div);
}

function addBondInput(name = '', price = '') {
    const list = document.getElementById('bond-list');
    const div = document.createElement('div');
    div.className = 'input-item';
    div.innerHTML = `
        <input type="text" placeholder="종목명" class="bond-name" value="${name}" style="flex: 1;">
        <input type="number" placeholder="평가금액" class="bond-price" value="${price}" style="flex: 1;">
        <button class="btn-danger" style="width: auto; padding: 0 12px; margin: 0;" onclick="this.parentElement.remove()">x</button>
    `;
    list.appendChild(div);
}

// Core Logic
function calculateAssets() {
    // 1. Parse Inputs
    assets.stocks = [];
    assets.bonds = [];
    assets.totalStock = 0;
    assets.totalBond = 0;

    // Stocks
    document.querySelectorAll('#stock-list .input-item').forEach(item => {
        const name = item.querySelector('.stock-name').value;
        const price = Number(item.querySelector('.stock-price').value) || 0;
        if (name || price > 0) {
            assets.stocks.push({ name, price });
            assets.totalStock += price;
        }
    });

    // Bonds
    document.querySelectorAll('#bond-list .input-item').forEach(item => {
        const name = item.querySelector('.bond-name').value;
        const price = Number(item.querySelector('.bond-price').value) || 0;
        if (name || price > 0) {
            assets.bonds.push({ name, price });
            assets.totalBond += price;
        }
    });

    // Gold
    assets.gold = Number(document.getElementById('gold-price').value) || 0;

    // Totals
    assets.total = assets.totalStock + assets.totalBond + assets.gold;

    // 2. Update Dashboard
    updateDashboard();

    // 3. Run Simulations
    runSimulation();

    // 4. Save
    saveData();
}

function updateDashboard() {
    document.getElementById('total-asset').textContent = formatCurrency(assets.total);
    
    document.getElementById('stock-asset').textContent = formatCurrency(assets.totalStock);
    document.getElementById('bond-asset').textContent = formatCurrency(assets.totalBond);
    document.getElementById('gold-asset').textContent = formatCurrency(assets.gold);

    const calcPercent = (val) => assets.total > 0 ? ((val / assets.total) * 100).toFixed(1) + '%' : '0%';

    document.getElementById('stock-percent').textContent = calcPercent(assets.totalStock);
    document.getElementById('bond-percent').textContent = calcPercent(assets.totalBond);
    document.getElementById('gold-percent').textContent = calcPercent(assets.gold);
}

function runSimulation() {
    if (assets.total === 0) return;

    // Simulation 1: Based on Stock (C code: rebal)
    // Logic: Bond = Stock * (3/6), Gold = Stock * (1/6) -> This seems to imply Stock is 50% relative weight?
    // C code: a = stock * 0.5, b = stock * (1/6)
    // Requirement for Bond: a - current_bond
    // Requirement for Gold: b - current_gold
    
    // Note: in C code:
    // a = all_stock * (3.0/6.0) => all_stock * 0.5
    // b = all_stock * (1.0/6.0)
    
    const targetBond1 = assets.totalStock * 0.5;
    const targetGold1 = assets.totalStock * (1.0 / 6.0);

    const diffBond1 = targetBond1 - assets.totalBond;
    const diffGold1 = targetGold1 - assets.gold;

    document.getElementById('rebal1-bond-target').textContent = formatCurrency(targetBond1);
    document.getElementById('rebal1-gold-target').textContent = formatCurrency(targetGold1);
    document.getElementById('rebal1-bond-diff').textContent = (diffBond1 > 0 ? '+' : '') + formatCurrency(diffBond1);
    document.getElementById('rebal1-gold-diff').textContent = (diffGold1 > 0 ? '+' : '') + formatCurrency(diffGold1);

    // Simulation 2: Fixed Ratio (C code: rebal2)
    // 60% Stock, 30% Bond, 10% Gold
    const targetStock2 = assets.total * 0.6;
    const targetBond2 = assets.total * 0.3;
    const targetGold2 = assets.total * 0.1;

    const diffStock2 = targetStock2 - assets.totalStock;
    const diffBond2 = targetBond2 - assets.totalBond;
    const diffGold2 = targetGold2 - assets.gold;

    document.getElementById('rebal2-stock-target').textContent = formatCurrency(targetStock2);
    document.getElementById('rebal2-bond-target').textContent = formatCurrency(targetBond2);
    document.getElementById('rebal2-gold-target').textContent = formatCurrency(targetGold2);

    document.getElementById('rebal2-stock-diff').textContent = (diffStock2 > 0 ? '+' : '') + formatCurrency(diffStock2);
    document.getElementById('rebal2-bond-diff').textContent = (diffBond2 > 0 ? '+' : '') + formatCurrency(diffBond2);
    document.getElementById('rebal2-gold-diff').textContent = (diffGold2 > 0 ? '+' : '') + formatCurrency(diffGold2);
}

// Persistence (f_save / f_load port)
function saveData() {
    localStorage.setItem('assetData', JSON.stringify(assets));
}

function loadData() {
    const saved = localStorage.getItem('assetData');
    if (saved) {
        assets = JSON.parse(saved);
        
        // Restore Inputs
        const stockList = document.getElementById('stock-list');
        stockList.innerHTML = '';
        assets.stocks.forEach(s => addStockInput(s.name, s.price));

        const bondList = document.getElementById('bond-list');
        bondList.innerHTML = '';
        assets.bonds.forEach(b => addBondInput(b.name, b.price));

        document.getElementById('gold-price').value = assets.gold;

        // Restore View
        updateDashboard();
        runSimulation();
    }
}

function resetData() {
    if(confirm('모든 데이터를 초기화하시겠습니까?')) {
        localStorage.removeItem('assetData');
        location.reload();
    }
}
