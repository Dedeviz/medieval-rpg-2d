const canvas = document.getElementById('gameCanvas');
const ctx = canvas.getContext('2d');
ctx.imageSmoothingEnabled = false;

const TILE = 16;
const MAP_WIDTH = 42;
const MAP_HEIGHT = 28;
const PLAYER_RADIUS = 0.28;

const ui = {
  hp: document.getElementById('hpValue'),
  level: document.getElementById('levelValue'),
  xp: document.getElementById('xpValue'),
  gold: document.getElementById('goldValue'),
  weapon: document.getElementById('weaponValue'),
  area: document.getElementById('areaName'),
};

const keys = {};
const pointer = { x: 0, y: 0 };

const areaNames = [
  'Valle di Brinco',
  'Bosco Nero',
  'Ciminiera del Gelo',
  'Lande di Ash',
  'Sotterranei di Ferro',
  'Castello delle Nevi',
];

const state = {
  time: 0,
  gameOver: false,
  mapLevel: 1,
  areaIndex: 0,
  player: null,
  map: null,
  enemies: [],
  drops: [],
  particles: [],
  attackFlash: 0,
  lastTime: 0,
};

function rand(min, max) {
  return Math.random() * (max - min) + min;
}

function clamp(value, min, max) {
  return Math.min(Math.max(value, min), max);
}

function choose(arr) {
  return arr[Math.floor(Math.random() * arr.length)];
}

function rect(x, y, w, h, color) {
  ctx.fillStyle = color;
  ctx.fillRect(Math.round(x), Math.round(y), Math.round(w), Math.round(h));
}

function distance(a, b) {
  return Math.hypot(a.x - b.x, a.y - b.y);
}

function normalize(x, y) {
  const len = Math.hypot(x, y) || 1;
  return { x: x / len, y: y / len };
}

function isWall(x, y) {
  const tx = Math.floor(x);
  const ty = Math.floor(y);
  if (tx < 0 || ty < 0 || tx >= state.map.width || ty >= state.map.height) return true;
  return state.map.tiles[ty][tx] === 1;
}

function canMoveTo(entity, nextX, nextY) {
  const r = entity.radius || PLAYER_RADIUS;
  return !(
    isWall(nextX - r, nextY - r) ||
    isWall(nextX + r, nextY - r) ||
    isWall(nextX - r, nextY + r) ||
    isWall(nextX + r, nextY + r)
  );
}

function generateMap(level) {
  const width = MAP_WIDTH;
  const height = MAP_HEIGHT;
  const tiles = Array.from({ length: height }, () => new Array(width).fill(1));
  const rooms = [];

  for (let i = 0; i < 9; i++) {
    const w = 4 + Math.floor(Math.random() * 6);
    const h = 4 + Math.floor(Math.random() * 5);
    const x = 1 + Math.floor(Math.random() * (width - w - 2));
    const y = 1 + Math.floor(Math.random() * (height - h - 2));

    rooms.push({ x, y, w, h });

    for (let yy = y; yy < y + h; yy++) {
      for (let xx = x; xx < x + w; xx++) {
        tiles[yy][xx] = 0;
      }
    }
  }

  for (let i = 1; i < rooms.length; i++) {
    const prev = rooms[i - 1];
    const curr = rooms[i];
    const startX = Math.floor(prev.x + prev.w / 2);
    const startY = Math.floor(prev.y + prev.h / 2);
    const endX = Math.floor(curr.x + curr.w / 2);
    const endY = Math.floor(curr.y + curr.h / 2);

    for (let x = Math.min(startX, endX); x <= Math.max(startX, endX); x++) tiles[startY][x] = 0;
    for (let y = Math.min(startY, endY); y <= Math.max(startY, endY); y++) tiles[y][endX] = 0;
  }

  const startRoom = rooms[0];
  const endRoom = rooms[rooms.length - 1];
  const startX = startRoom.x + Math.floor(startRoom.w / 2) + 0.5;
  const startY = startRoom.y + Math.floor(startRoom.h / 2) + 0.5;
  const stairX = endRoom.x + Math.floor(endRoom.w / 2) + 0.5;
  const stairY = endRoom.y + Math.floor(endRoom.h / 2) + 0.5;

  return {
    width,
    height,
    tiles,
    rooms,
    start: { x: startX, y: startY },
    stairs: { x: stairX, y: stairY },
  };
}

function createPlayer() {
  return {
    x: state.map.start.x,
    y: state.map.start.y,
    radius: PLAYER_RADIUS,
    hp: 100,
    maxHp: 100,
    speed: 2.4,
    dirX: 1,
    dirY: 0,
    attackCooldown: 0,
    level: 1,
    xp: 0,
    nextXp: 10,
    gold: 0,
    weaponName: 'Spada Arrugginita',
    damage: 10,
    hitFlash: 0,
    potions: 1,
    lastMove: { x: 1, y: 0 },
  };
}

function createEnemy(x, y, level) {
  const base = 4 + level; 
  return {
    x,
    y,
    radius: 0.32,
    hp: 18 + level * 8,
    maxHp: 18 + level * 8,
    speed: 0.8 + level * 0.1,
    damage: 6 + level * 2,
    xp: 6 + level * 2,
    color: choose(['#d64933', '#c04e8a', '#6cc36f', '#77d5e8', '#d7a35b']),
    attackCooldown: 0,
    alive: true,
  };
}

function spawnEnemies(level) {
  const count = 8 + level * 2;
  const enemies = [];
  for (let i = 0; i < count; i++) {
    let px = rand(2, MAP_WIDTH - 2);
    let py = rand(2, MAP_HEIGHT - 2);
    let tries = 0;
    while (isWall(px, py) || distance({ x: px, y: py }, state.map.start) < 4 && tries < 120) {
      px = rand(2, MAP_WIDTH - 2);
      py = rand(2, MAP_HEIGHT - 2);
      tries++;
    }
    if (tries < 120) enemies.push(createEnemy(px, py, level));
  }
  state.enemies = enemies;
}

function spawnDrops() {
  state.drops = [];
  for (let i = 0; i < 7; i++) {
    let x = rand(2, MAP_WIDTH - 2);
    let y = rand(2, MAP_HEIGHT - 2);
    let tries = 0;
    while (isWall(x, y) || distance({ x, y }, state.map.start) < 2 && tries < 120) {
      x = rand(2, MAP_WIDTH - 2);
      y = rand(2, MAP_HEIGHT - 2);
      tries++;
    }
    const roll = Math.random();
    if (roll < 0.5) {
      state.drops.push({ x, y, type: 'gold', amount: 12 + Math.floor(Math.random() * 20), color: '#f7d36f' });
    } else if (roll < 0.75) {
      state.drops.push({ x, y, type: 'potion', amount: 1, color: '#63d3d5' });
    } else {
      state.drops.push({ x, y, type: 'weapon', amount: 1, color: '#f2976b' });
    }
  }
}

function levelUp() {
  state.player.level += 1;
  state.player.damage += 2;
  state.player.maxHp += 12;
  state.player.hp = state.player.maxHp;
  state.player.potions += 1;
  state.player.nextXp = Math.floor(state.player.nextXp * 1.5) + 5;
}

function gainXp(amount) {
  state.player.xp += amount;
  while (state.player.xp >= state.player.nextXp) {
    state.player.xp -= state.player.nextXp;
    levelUp();
  }
}

function usePotion() {
  if (state.player.potions > 0 && state.player.hp < state.player.maxHp) {
    state.player.potions -= 1;
    state.player.hp = Math.min(state.player.maxHp, state.player.hp + 28);
  }
}

function triggerAttack() {
  if (state.player.attackCooldown > 0 || state.gameOver) return;

  const p = state.player;
  const dir = { x: p.lastMove.x || 1, y: p.lastMove.y || 0 };
  const hitBox = 0.9;
  const attackRange = 1.1;

  p.attackCooldown = 0.35;
  state.attackFlash = 0.14;

  for (const enemy of state.enemies) {
    if (!enemy.alive) continue;
    const dx = enemy.x - p.x;
    const dy = enemy.y - p.y;
    const dot = dx * dir.x + dy * dir.y;
    const dist = Math.hypot(dx, dy);
    const faceValid = dot > 0;
    const inArc = Math.acos(clamp(dot / (dist || 1), -1, 1)) < 0.9;

    if (faceValid && dist < attackRange && inArc) {
      enemy.hp -= p.damage;
      if (enemy.hp <= 0) {
        enemy.alive = false;
        gainXp(enemy.xp);
        const chance = Math.random();
        if (chance < 0.3) {
          state.drops.push({ x: enemy.x, y: enemy.y, type: 'gold', amount: 8 + Math.floor(Math.random() * 12), color: '#f7d36f' });
        }
      }
      createBurst(enemy.x, enemy.y, '#ffa76d', 8);
    }
  }
}

function createBurst(x, y, color, count = 8) {
  for (let i = 0; i < count; i++) {
    state.particles.push({
      x,
      y,
      dx: rand(-0.18, 0.18),
      dy: rand(-0.18, 0.18),
      life: rand(0.3, 0.7),
      color,
      size: rand(1.5, 3),
    });
  }
}

function tryCollectDrops() {
  const p = state.player;
  for (let i = state.drops.length - 1; i >= 0; i--) {
    const drop = state.drops[i];
    if (distance(p, drop) < 0.6) {
      if (drop.type === 'gold') {
        state.player.gold += drop.amount;
      } else if (drop.type === 'potion') {
        state.player.potions += 1;
      } else if (drop.type === 'weapon') {
        state.player.damage += 3;
        state.player.weaponName = 'Spada del Millepiani';
      }
      state.drops.splice(i, 1);
      createBurst(drop.x, drop.y, '#d5f7a5', 10);
    }
  }
}

function moveEnemy(enemy, dt) {
  if (!enemy.alive) return;
  const dx = state.player.x - enemy.x;
  const dy = state.player.y - enemy.y;
  const dist = Math.hypot(dx, dy) || 1;
  if (dist > 0.2) {
    const nx = dx / dist;
    const ny = dy / dist;
    const nextX = enemy.x + nx * enemy.speed * dt;
    const nextY = enemy.y + ny * enemy.speed * dt;
    if (canMoveTo(enemy, nextX, enemy.y)) enemy.x = nextX;
    if (canMoveTo(enemy, enemy.x, nextY)) enemy.y = nextY;
  }

  if (dist < 0.85 && enemy.attackCooldown <= 0) {
    state.player.hp -= enemy.damage;
    state.player.hitFlash = 0.26;
    enemy.attackCooldown = 1.0;
    createBurst(state.player.x, state.player.y, '#ff7b7b', 12);
    if (state.player.hp <= 0) {
      state.gameOver = true;
    }
  }
}

function buildNextLevel() {
  state.mapLevel += 1;
  state.areaIndex = (state.areaIndex + 1) % areaNames.length;
  state.map = generateMap(state.mapLevel);
  state.player.x = state.map.start.x;
  state.player.y = state.map.start.y;
  state.player.hp = Math.min(state.player.maxHp, state.player.hp + 14);
  spawnEnemies(state.mapLevel);
  spawnDrops();
  ui.area.textContent = areaNames[state.areaIndex];
}

function checkStairs() {
  const p = state.player;
  if (distance(p, state.map.stairs) < 0.7) {
    buildNextLevel();
  }
}

function update(dt) {
  if (state.gameOver) return;

  state.time += dt;
  const p = state.player;
  p.attackCooldown = Math.max(0, p.attackCooldown - dt);
  p.hitFlash = Math.max(0, p.hitFlash - dt);

  let moveX = 0;
  let moveY = 0;

  if (keys['KeyW'] || keys['ArrowUp']) moveY -= 1;
  if (keys['KeyS'] || keys['ArrowDown']) moveY += 1;
  if (keys['KeyA'] || keys['ArrowLeft']) moveX -= 1;
  if (keys['KeyD'] || keys['ArrowRight']) moveX += 1;

  if (moveX !== 0 || moveY !== 0) {
    const norm = normalize(moveX, moveY);
    const nextX = p.x + norm.x * p.speed * dt;
    const nextY = p.y + norm.y * p.speed * dt;
    if (canMoveTo(p, nextX, p.y)) p.x = nextX;
    if (canMoveTo(p, p.x, nextY)) p.y = nextY;
    p.lastMove.x = norm.x;
    p.lastMove.y = norm.y;
  }

  for (const enemy of state.enemies) {
    if (enemy.alive) {
      moveEnemy(enemy, dt);
      enemy.attackCooldown = Math.max(0, enemy.attackCooldown - dt);
    }
  }

  tryCollectDrops();
  checkStairs();

  for (let i = state.particles.length - 1; i >= 0; i--) {
    const part = state.particles[i];
    part.x += part.dx;
    part.y += part.dy;
    part.life -= dt;
    if (part.life <= 0) state.particles.splice(i, 1);
  }

  if (state.player.hp > state.player.maxHp) state.player.hp = state.player.maxHp;
  updateUI();
}

function updateUI() {
  const p = state.player;
  ui.hp.textContent = `${Math.max(0, Math.floor(p.hp))} / ${p.maxHp}`;
  ui.level.textContent = String(p.level);
  ui.xp.textContent = `${p.xp} / ${p.nextXp}`;
  ui.gold.textContent = String(p.gold);
  ui.weapon.textContent = p.weaponName;
}

function drawTileMap() {
  const cameraX = Math.floor(state.player.x * TILE - canvas.width / 2);
  const cameraY = Math.floor(state.player.y * TILE - canvas.height / 2);

  for (let y = 0; y < state.map.height; y++) {
    for (let x = 0; x < state.map.width; x++) {
      const screenX = x * TILE - cameraX;
      const screenY = y * TILE - cameraY;
      const isVisible = screenX >= -TILE && screenX <= canvas.width && screenY >= -TILE && screenY <= canvas.height;
      if (!isVisible) continue;

      const tile = state.map.tiles[y][x];
      if (tile === 0) {
        rect(screenX, screenY, TILE, TILE, (x + y) % 2 === 0 ? '#2a2c34' : '#252931');
        rect(screenX + 2, screenY + 2, TILE - 4, TILE - 4, (x + y) % 2 === 0 ? '#404a54' : '#3c4651');
      } else {
        rect(screenX, screenY, TILE, TILE, '#392d2d');
        rect(screenX + 1, screenY + 1, TILE - 2, TILE - 2, '#584b46');
      }
    }
  }

  const stairsScreenX = state.map.stairs.x * TILE - cameraX;
  const stairsScreenY = state.map.stairs.y * TILE - cameraY;
  rect(stairsScreenX - 4, stairsScreenY - 4, 8, 8, '#f8d57a');
  rect(stairsScreenX - 2, stairsScreenY - 2, 4, 4, '#fef3b7');
}

function drawActor(actor, color, size = 12) {
  const x = actor.x * TILE - (state.player.x * TILE - canvas.width / 2);
  const y = actor.y * TILE - (state.player.y * TILE - canvas.height / 2);
  rect(x - size / 2, y - size / 2, size, size, color);

  if (actor.hitFlash > 0) {
    rect(x - size / 2 - 1, y - size / 2 - 1, size + 2, size + 2, 'rgba(255,255,255,0.5)');
  }
}

function drawDrops() {
  for (const drop of state.drops) {
    const x = drop.x * TILE - (state.player.x * TILE - canvas.width / 2);
    const y = drop.y * TILE - (state.player.y * TILE - canvas.height / 2);
    rect(x - 3, y - 3, 6, 6, drop.color);
  }
}

function drawParticles() {
  for (const part of state.particles) {
    const x = part.x * TILE - (state.player.x * TILE - canvas.width / 2);
    const y = part.y * TILE - (state.player.y * TILE - canvas.height / 2);
    rect(x, y, part.size, part.size, part.color);
  }
}

function drawPlayer() {
  const p = state.player;
  const x = canvas.width / 2;
  const y = canvas.height / 2;
  const size = 12;

  rect(x - size / 2, y - size / 2, size, size, p.hitFlash > 0 ? '#fff6d6' : '#7ae39d');

  const dir = p.lastMove;
  const beamX = x + dir.x * 10;
  const beamY = y + dir.y * 10;
  if (state.attackFlash > 0) {
    rect(beamX - 3, beamY - 3, 6, 6, '#f4d35e');
  }
}

function drawHudInset() {
  const px = canvas.width / 2;
  const py = canvas.height / 2;
  const p = state.player;

  rect(px - 52, 18, 104, 16, 'rgba(11, 11, 17, 0.7)');
  rect(px - 50, 20, 100 * (p.hp / p.maxHp), 12, '#ff6b6b');

  if (state.gameOver) {
    rect(canvas.width / 2 - 160, canvas.height / 2 - 60, 320, 120, 'rgba(10, 12, 18, 0.8)');
    ctx.fillStyle = '#f7e7b6';
    ctx.font = 'bold 30px Trebuchet MS';
    ctx.textAlign = 'center';
    ctx.fillText('RUNE FALLEN', canvas.width / 2, canvas.height / 2 - 10);
    ctx.font = '18px Trebuchet MS';
    ctx.fillText('Premi R per ricominciare', canvas.width / 2, canvas.height / 2 + 25);
  }
}

function draw() {
  ctx.clearRect(0, 0, canvas.width, canvas.height);
  drawTileMap();
  drawDrops();
  drawParticles();

  for (const enemy of state.enemies) {
    if (enemy.alive) {
      drawActor(enemy, enemy.color, 12);
      const x = enemy.x * TILE - (state.player.x * TILE - canvas.width / 2);
      const y = enemy.y * TILE - (state.player.y * TILE - canvas.height / 2);
      rect(x - 7, y - 12, 14, 3, 'rgba(8,8,14,0.6)');
      rect(x - 6, y - 11, 12 * (enemy.hp / enemy.maxHp), 2, '#ff7a75');
    }
  }

  drawPlayer();
  drawHudInset();
}

function resetGame() {
  state.mapLevel = 1;
  state.areaIndex = 0;
  state.map = generateMap(1);
  state.player = createPlayer();
  state.enemies = [];
  state.drops = [];
  state.particles = [];
  state.gameOver = false;
  state.attackFlash = 0;
  ui.area.textContent = areaNames[0];
  spawnEnemies(1);
  spawnDrops();
  updateUI();
}

window.addEventListener('keydown', (event) => {
  keys[event.code] = true;
  if (event.code === 'Space') {
    event.preventDefault();
    triggerAttack();
  }
  if (event.code === 'KeyE') {
    usePotion();
  }
  if (event.code === 'KeyR' && state.gameOver) {
    resetGame();
  }
});

window.addEventListener('keyup', (event) => {
  keys[event.code] = false;
});

window.addEventListener('mousemove', (event) => {
  const rect = canvas.getBoundingClientRect();
  pointer.x = ((event.clientX - rect.left) / rect.width) * canvas.width;
  pointer.y = ((event.clientY - rect.top) / rect.height) * canvas.height;
});

let lastFrame = 0;
function loop(timestamp) {
  const dt = Math.min((timestamp - lastFrame) / 1000 || 0.016, 0.033);
  lastFrame = timestamp;
  update(dt);
  draw();
  requestAnimationFrame(loop);
}

resetGame();
requestAnimationFrame(loop);
