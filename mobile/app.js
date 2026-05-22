// ===== STATE =====
let state = {
  stocks: [], bonds: [], gold: 0, cash: 0,
  targetStock: 60, targetBond: 30, targetGold: 10,
  simAll: 0
};

function getAll() { return getAllStock() + getAllBond() + state.gold + state.cash; }
function getAllStock() { return state.stocks.reduce((s, x) => s + x.price, 0); }
function getAllBond() { return state.bonds.reduce((s, x) => s + x.price, 0); }
function fmt(n) { return Math.round(n).toLocaleString('ko-KR'); }
function pct(part, total) { return total > 0 ? (part / total * 100) : 0; }

// ===== STORAGE =====
function save() { localStorage.setItem('assetData', JSON.stringify(state)); }
function load() {
  const d = localStorage.getItem('assetData');
  if (d) { try { state = JSON.parse(d); } catch(e) {} }
}

function exportJSON() {
  const blob = new Blob([JSON.stringify(state, null, 2)], {type: 'application/json'});
  const a = document.createElement('a'); a.href = URL.createObjectURL(blob);
  a.download = 'asset_data.json'; a.click();
}

function importJSON(file) {
  const reader = new FileReader();
  reader.onload = e => {
    try { state = JSON.parse(e.target.result); save(); renderAll(); alert('가져오기 성공!'); }
    catch(err) { alert('JSON 파싱 오류'); }
  };
  reader.readAsText(file);
}

function importSaveTxt(file) {
  const reader = new FileReader();
  reader.onload = e => {
    try {
      const lines = e.target.result.trim().split('\n');
      const vals = lines[1].trim().split(/\s+/).map(Number);
      const counts = lines[2].trim().split(/\s+/).map(Number);
      state.stocks = []; state.bonds = [];
      for (let i = 0; i < counts[0]; i++) {
        const p = lines[3 + i].trim().split(/\s+/);
        state.stocks.push({name: p[0], price: Number(p[1]), goal: Number(p[2])});
      }
      for (let i = 0; i < counts[1]; i++) {
        const p = lines[3 + counts[0] + i].trim().split(/\s+/);
        state.bonds.push({name: p[0], price: Number(p[1]), goal: Number(p[2])});
      }
      state.gold = vals[3]; state.cash = vals[4];
      save(); renderAll(); alert('save.txt 가져오기 성공!');
    } catch(err) { alert('파싱 오류: ' + err.message); }
  };
  reader.readAsText(file);
}

// ===== NAVIGATION =====
function switchTab(id) {
  document.querySelectorAll('.tab-panel').forEach(p => p.classList.remove('active'));
  document.querySelectorAll('.nav-item').forEach(n => n.classList.remove('active'));
  document.getElementById('tab-' + id).classList.add('active');
  document.querySelector(`[data-tab="${id}"]`).classList.add('active');
  if (id === 'dashboard') renderDashboard();
  if (id === 'rebal') renderRebal();
  if (id === 'chart') renderChart();
  if (id === 'settings') renderSettings();
}

// ===== RENDER: DASHBOARD =====
function renderDashboard() {
  const all = getAll(), as = getAllStock(), ab = getAllBond();
  const el = document.getElementById('tab-dashboard');
  const ps = pct(as, all), pb = pct(ab, all), pg = pct(state.gold, all), pc = pct(state.cash, all);
  let warn = '';
  if (all > 0) {
    const ds = Math.abs(ps - state.targetStock), db = Math.abs(pb - state.targetBond), dg = Math.abs(pg - state.targetGold);
    if (ds > 5 || db > 5 || dg > 5) warn = `<div class="warning-banner">⚠️ 목표 비중 대비 5% 이상 편차 발생! 리밸런싱을 검토하세요.</div>`;
  }
  el.innerHTML = all > 0 ? `
    <div class="hero">
      <div class="hero-label">총 자산</div>
      <div class="hero-value">${fmt(all)}원</div>
    </div>
    ${warn}
    <div class="cat-grid">
      <div class="cat-card stock"><div class="cat-label">주식</div><div class="cat-value">${fmt(as)}원</div><div class="cat-pct">${ps.toFixed(1)}% (목표 ${state.targetStock}%)</div></div>
      <div class="cat-card bond"><div class="cat-label">채권</div><div class="cat-value">${fmt(ab)}원</div><div class="cat-pct">${pb.toFixed(1)}% (목표 ${state.targetBond}%)</div></div>
      <div class="cat-card gold"><div class="cat-label">금</div><div class="cat-value">${fmt(state.gold)}원</div><div class="cat-pct">${pg.toFixed(1)}% (목표 ${state.targetGold}%)</div></div>
      <div class="cat-card cash"><div class="cat-label">예수금</div><div class="cat-value">${fmt(state.cash)}원</div><div class="cat-pct">${pc.toFixed(1)}%</div></div>
    </div>
    <div class="alloc-bar-wrap">
      <div class="alloc-bar-label">현재 비중</div>
      <div class="alloc-bar"><div class="seg-stock" style="width:${ps}%"></div><div class="seg-bond" style="width:${pb}%"></div><div class="seg-gold" style="width:${pg}%"></div><div class="seg-cash" style="width:${pc}%"></div></div>
    </div>
    <div class="alloc-bar-wrap">
      <div class="alloc-bar-label">목표 비중</div>
      <div class="alloc-bar"><div class="seg-stock" style="width:${state.targetStock}%"></div><div class="seg-bond" style="width:${state.targetBond}%"></div><div class="seg-gold" style="width:${state.targetGold}%"></div></div>
    </div>
  ` : `<div class="empty-state"><div class="icon">📊</div><p>자산 데이터가 없습니다.<br>입력 탭에서 자산을 추가하세요.</p></div>`;
}

// ===== RENDER: INPUT =====
function renderInput() {
  const el = document.getElementById('tab-input');
  let html = '<div class="section-title"><div class="dot" style="background:var(--stock)"></div>주식 종목</div>';
  state.stocks.forEach((s, i) => {
    html += `<div class="asset-item"><button class="remove-btn" onclick="removeStock(${i})">✕</button>
      <div class="input-row-3">
        <div class="input-group"><label>종목명</label><input value="${s.name}" onchange="updStock(${i},'name',this.value)"></div>
        <div class="input-group"><label>평가금액</label><input type="number" value="${s.price}" onchange="updStock(${i},'price',+this.value)"></div>
        <div class="input-group"><label>목표%</label><input type="number" value="${s.goal}" onchange="updStock(${i},'goal',+this.value)"></div>
      </div></div>`;
  });
  html += `<button class="add-btn" onclick="addStock()">+ 주식 종목 추가</button>`;
  html += '<div class="section-title"><div class="dot" style="background:var(--bond)"></div>채권 종목</div>';
  state.bonds.forEach((b, i) => {
    html += `<div class="asset-item"><button class="remove-btn" onclick="removeBond(${i})">✕</button>
      <div class="input-row-3">
        <div class="input-group"><label>종목명</label><input value="${b.name}" onchange="updBond(${i},'name',this.value)"></div>
        <div class="input-group"><label>평가금액</label><input type="number" value="${b.price}" onchange="updBond(${i},'price',+this.value)"></div>
        <div class="input-group"><label>목표%</label><input type="number" value="${b.goal}" onchange="updBond(${i},'goal',+this.value)"></div>
      </div></div>`;
  });
  html += `<button class="add-btn" onclick="addBond()">+ 채권 종목 추가</button>`;
  html += `<div class="section-title"><div class="dot" style="background:var(--gold)"></div>기타 자산</div>
    <div class="card"><div class="input-row">
      <div class="input-group"><label>금 현물 ETF</label><input type="number" value="${state.gold}" onchange="state.gold=+this.value;save()"></div>
      <div class="input-group"><label>예수금</label><input type="number" value="${state.cash}" onchange="state.cash=+this.value;save()"></div>
    </div></div>`;
  const all = getAll();
  if (all > 0) html += `<div class="info-banner">총 자산: ${fmt(all)}원</div>`;
  el.innerHTML = html;
}
function addStock() { state.stocks.push({name:'', price:0, goal:0}); save(); renderInput(); }
function removeStock(i) { state.stocks.splice(i,1); save(); renderInput(); }
function updStock(i,k,v) { state.stocks[i][k]=v; save(); }
function addBond() { state.bonds.push({name:'', price:0, goal:0}); save(); renderInput(); }
function removeBond(i) { state.bonds.splice(i,1); save(); renderInput(); }
function updBond(i,k,v) { state.bonds[i][k]=v; save(); }

// ===== RENDER: REBALANCING =====
function renderRebal() {
  const all = getAll(), as = getAllStock(), ab = getAllBond();
  const el = document.getElementById('tab-rebal');
  if (all <= 0) { el.innerHTML = '<div class="empty-state"><div class="icon">⚖️</div><p>자산 데이터를 먼저 입력하세요.</p></div>'; return; }

  // 1) rebal: 주식 대비 채권/금
  const needBond = as * (3.0/6.0) - ab, needGold = as * (1.0/6.0) - state.gold;
  // 2) rebal2: 총자산 기준
  const simAll = state.simAll > 0 ? all + state.simAll : all;
  const tStock = simAll * state.targetStock / 100;
  const tBond = simAll * state.targetBond / 100;
  const tGold = simAll * state.targetGold / 100;

  let html = `
    <div class="collapse-header open" onclick="toggleCollapse(this)">📐 주식 대비 채권/금 차이<span class="arrow">▼</span></div>
    <div class="collapse-body open"><div class="result-card">
      <div class="result-row"><span class="result-label">주식(고정) 대비 채권 필요량</span><span class="result-value ${needBond>=0?'pos':'neg'}">${needBond>=0?'+':''}${fmt(needBond)}원</span></div>
      <div class="result-row"><span class="result-label">주식(고정) 대비 금 필요량</span><span class="result-value ${needGold>=0?'pos':'neg'}">${needGold>=0?'+':''}${fmt(needGold)}원</span></div>
    </div></div>

    <div class="collapse-header open" onclick="toggleCollapse(this)">🎯 총 자산 기준 시뮬레이션<span class="arrow">▼</span></div>
    <div class="collapse-body open">
      <div class="seed-area">
        <div class="input-group"><label>추가 시드</label><input type="number" id="seedInput" value="${state.simAll||''}" placeholder="0"></div>
        <button class="btn btn-primary" onclick="state.simAll=+(document.getElementById('seedInput').value||0);save();renderRebal()">적용</button>
      </div>
      <div class="result-card">
        <div class="result-row"><span class="result-label">시뮬 총 자산</span><span class="result-value">${fmt(simAll)}원</span></div>
        <div class="result-row"><span class="result-label">주식 목표 (${state.targetStock}%)</span><span class="result-value">${fmt(tStock)}원</span></div>
        <div class="result-row"><span class="result-label">채권 목표 (${state.targetBond}%)</span><span class="result-value">${fmt(tBond)}원</span></div>
        <div class="result-row"><span class="result-label">금 목표 (${state.targetGold}%)</span><span class="result-value">${fmt(tGold)}원</span></div>
        <hr style="border-color:var(--card-border);margin:8px 0">
        <div class="result-row"><span class="result-label">주식 과부족</span><span class="result-value ${tStock-as>=0?'pos':'neg'}">${tStock-as>=0?'+':''}${fmt(tStock-as)}원</span></div>
        <div class="result-row"><span class="result-label">채권 과부족</span><span class="result-value ${tBond-ab>=0?'pos':'neg'}">${tBond-ab>=0?'+':''}${fmt(tBond-ab)}원</span></div>
        <div class="result-row"><span class="result-label">금 과부족</span><span class="result-value ${tGold-state.gold>=0?'pos':'neg'}">${tGold-state.gold>=0?'+':''}${fmt(tGold-state.gold)}원</span></div>
      </div>
    </div>`;

  // 3) rebal_stock
  html += `<div class="collapse-header" onclick="toggleCollapse(this)">📈 개별 주식 비중<span class="arrow">▼</span></div><div class="collapse-body">`;
  if (state.stocks.length > 0) {
    const base = simAll > all ? simAll : all;
    html += '<div class="result-card" style="overflow-x:auto"><table class="data-table"><tr><th>종목</th><th>목표</th><th>현재</th><th>과부족</th></tr>';
    state.stocks.forEach(s => {
      const cp = pct(s.price, all), diff = base * s.goal / 100 - s.price;
      html += `<tr><td>${s.name}</td><td>${s.goal}%</td><td>${fmt(s.price)}원 (${cp.toFixed(1)}%)</td><td class="${diff>=0?'pos':'neg'}">${diff>=0?'+':''}${fmt(diff)}원</td></tr>`;
    });
    html += '</table></div>';
  } else html += '<div class="info-banner">주식 데이터 없음</div>';
  html += '</div>';

  // 4) rebal_bond
  html += `<div class="collapse-header" onclick="toggleCollapse(this)">📉 개별 채권 비중<span class="arrow">▼</span></div><div class="collapse-body">`;
  if (state.bonds.length > 0) {
    const base = simAll > all ? simAll : all;
    html += '<div class="result-card" style="overflow-x:auto"><table class="data-table"><tr><th>종목</th><th>목표</th><th>현재</th><th>과부족</th></tr>';
    state.bonds.forEach(b => {
      const cp = pct(b.price, all), diff = base * b.goal / 100 - b.price;
      html += `<tr><td>${b.name}</td><td>${b.goal}%</td><td>${fmt(b.price)}원 (${cp.toFixed(1)}%)</td><td class="${diff>=0?'pos':'neg'}">${diff>=0?'+':''}${fmt(diff)}원</td></tr>`;
    });
    html += '</table></div>';
  } else html += '<div class="info-banner">채권 데이터 없음</div>';
  html += '</div>';

  el.innerHTML = html;
}
function toggleCollapse(el) {
  el.classList.toggle('open');
  el.nextElementSibling.classList.toggle('open');
}

// ===== TREEMAP (squarify) =====
function squarify(items, rect) {
  if (!items.length) return [];
  const total = items.reduce((s,d) => s+d.value, 0);
  if (total <= 0) return [];
  const areas = items.map(d => ({...d, area: d.value/total*rect.w*rect.h})).sort((a,b) => b.area-a.area);
  const res = []; layoutRow(areas, 0, rect, res); return res;
}
function layoutRow(items, start, rect, res) {
  if (start >= items.length) return;
  if (start === items.length-1) { res.push({...items[start], x:rect.x, y:rect.y, w:rect.w, h:rect.h}); return; }
  if (rect.w <= 0 || rect.h <= 0) return;
  const shorter = Math.min(rect.w, rect.h);
  let rowEnd = start, bestW = Infinity;
  for (let i = start; i < items.length; i++) {
    const w = worstRatio(items, start, i, shorter);
    if (w <= bestW) { bestW = w; rowEnd = i; } else break;
  }
  const rowArea = items.slice(start, rowEnd+1).reduce((s,d) => s+d.area, 0);
  if (rect.w >= rect.h) {
    const rw = rowArea / rect.h; let y = rect.y;
    for (let i = start; i <= rowEnd; i++) { const h = items[i].area/rw; res.push({...items[i], x:rect.x, y, w:rw, h}); y += h; }
    layoutRow(items, rowEnd+1, {x:rect.x+rw, y:rect.y, w:rect.w-rw, h:rect.h}, res);
  } else {
    const rh = rowArea / rect.w; let x = rect.x;
    for (let i = start; i <= rowEnd; i++) { const w = items[i].area/rh; res.push({...items[i], x, y:rect.y, w, h:rh}); x += w; }
    layoutRow(items, rowEnd+1, {x:rect.x, y:rect.y+rh, w:rect.w, h:rect.h-rh}, res);
  }
}
function worstRatio(items, start, end, shorter) {
  const rowArea = items.slice(start, end+1).reduce((s,d) => s+d.area, 0);
  const len = rowArea / shorter; let worst = 0;
  for (let i = start; i <= end; i++) {
    const other = items[i].area / len;
    worst = Math.max(worst, Math.max(len/other, other/len));
  }
  return worst;
}

// ===== RENDER: CHART =====
function renderChart() {
  const el = document.getElementById('tab-chart');
  const all = getAll();
  if (all <= 0) { el.innerHTML = '<div class="empty-state"><div class="icon">📊</div><p>차트를 표시할 데이터가 없습니다.</p></div>'; return; }

  const items = [];
  const stockColors = ['#ef4444','#f97316','#fb923c','#f87171','#dc2626','#c2410c','#ea580c','#e11d48'];
  const bondColors = ['#3b82f6','#06b6d4','#60a5fa','#38bdf8','#2563eb','#0891b2'];
  state.stocks.forEach((s,i) => { if (s.price > 0) items.push({name:s.name, value:s.price, color:stockColors[i%stockColors.length], type:'주식'}); });
  state.bonds.forEach((b,i) => { if (b.price > 0) items.push({name:b.name, value:b.price, color:bondColors[i%bondColors.length], type:'채권'}); });
  if (state.gold > 0) items.push({name:'Gold', value:state.gold, color:'#f59e0b', type:'금'});
  if (state.cash > 0) items.push({name:'예수금', value:state.cash, color:'#10b981', type:'현금'});

  const container = document.createElement('div');
  el.innerHTML = '<div class="section-title">📊 자산 배분 히트맵</div>';
  const tm = document.createElement('div');
  tm.className = 'treemap-container';
  tm.id = 'treemapBox';
  el.appendChild(tm);

  // Legend
  let legend = '<div style="display:flex;flex-wrap:wrap;gap:8px;margin-top:8px;margin-bottom:16px">';
  legend += '<span style="font-size:.7rem;display:flex;align-items:center;gap:4px"><span style="width:10px;height:10px;border-radius:2px;background:var(--stock);display:inline-block"></span>주식</span>';
  legend += '<span style="font-size:.7rem;display:flex;align-items:center;gap:4px"><span style="width:10px;height:10px;border-radius:2px;background:var(--bond);display:inline-block"></span>채권</span>';
  legend += '<span style="font-size:.7rem;display:flex;align-items:center;gap:4px"><span style="width:10px;height:10px;border-radius:2px;background:var(--gold);display:inline-block"></span>금</span>';
  legend += '<span style="font-size:.7rem;display:flex;align-items:center;gap:4px"><span style="width:10px;height:10px;border-radius:2px;background:var(--cash);display:inline-block"></span>예수금</span>';
  legend += '</div>';
  el.insertAdjacentHTML('beforeend', legend);

  requestAnimationFrame(() => {
    const box = document.getElementById('treemapBox');
    const W = box.clientWidth, H = box.clientHeight;
    const rects = squarify(items, {x:0, y:0, w:W, h:H});
    rects.forEach(r => {
      const cell = document.createElement('div');
      cell.className = 'treemap-cell';
      cell.style.cssText = `left:${r.x}px;top:${r.y}px;width:${r.w}px;height:${r.h}px;background:${r.color};`;
      const p = pct(r.value, all);
      if (r.w > 50 && r.h > 30) cell.innerHTML = `<span class="tm-name">${r.name}</span><span class="tm-val">${fmt(r.value)}원</span><span class="tm-pct">${p.toFixed(1)}%</span>`;
      else if (r.w > 30 && r.h > 20) cell.innerHTML = `<span class="tm-name" style="font-size:.55rem">${r.name}</span>`;
      box.appendChild(cell);
    });
  });
}

// ===== RENDER: SETTINGS =====
function renderSettings() {
  const el = document.getElementById('tab-settings');
  const sum = state.targetStock + state.targetBond + state.targetGold;
  el.innerHTML = `
    <div class="section-title">🎯 목표 비율 설정</div>
    <div class="card">
      <div class="slider-group"><label><span style="color:var(--stock)">주식</span><span>${state.targetStock}%</span></label>
        <input type="range" min="0" max="100" value="${state.targetStock}" oninput="state.targetStock=+this.value;save();renderSettings()"></div>
      <div class="slider-group"><label><span style="color:var(--bond)">채권</span><span>${state.targetBond}%</span></label>
        <input type="range" min="0" max="100" value="${state.targetBond}" oninput="state.targetBond=+this.value;save();renderSettings()"></div>
      <div class="slider-group"><label><span style="color:var(--gold)">금</span><span>${state.targetGold}%</span></label>
        <input type="range" min="0" max="100" value="${state.targetGold}" oninput="state.targetGold=+this.value;save();renderSettings()"></div>
      <div class="ratio-sum ${sum===100?'ok':'bad'}">합계: ${sum}% ${sum===100?'✓':'(100%가 되어야 합니다)'}</div>
    </div>
    <div class="section-title">💾 데이터 관리</div>
    <button class="btn btn-primary" onclick="exportJSON()">📤 JSON으로 내보내기</button>
    <button class="btn btn-outline" onclick="document.getElementById('importJson').click()">📥 JSON 가져오기</button>
    <input type="file" id="importJson" accept=".json" style="display:none" onchange="importJSON(this.files[0])">
    <button class="btn btn-outline" onclick="document.getElementById('importTxt').click()">📄 save.txt 가져오기</button>
    <input type="file" id="importTxt" accept=".txt" style="display:none" onchange="importSaveTxt(this.files[0])">
    <button class="btn btn-danger" onclick="if(confirm('모든 데이터를 삭제하시겠습니까?')){localStorage.clear();state={stocks:[],bonds:[],gold:0,cash:0,targetStock:60,targetBond:30,targetGold:10,simAll:0};renderAll();}">🗑️ 데이터 초기화</button>
  `;
}

function renderAll() { renderDashboard(); renderInput(); }

// ===== INIT =====
window.addEventListener('DOMContentLoaded', () => {
  load(); renderAll(); switchTab('dashboard');
});
