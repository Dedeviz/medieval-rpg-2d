const canvas = document.getElementById('gameCanvas');
const ctx = canvas.getContext('2d');
ctx.imageSmoothingEnabled = false;

const TILE = 16;
const MAP_W = 30;
const MAP_H = 20;
const PLAYER_RADIUS = 0.32;

const ui = {
  hp: document.getElementById('hpValue'),
  level: document.getElementById('levelValue'),
  xp: document.getElementById('xpValue'),
  gold: document.getElementById('goldValue'),
  weapon: document.getElementById('weaponValue'),
  area: document.getElementById('areaName'),
  inventory: document.getElementById('inventoryList'),
  equipment: document.getElementById('equipmentList'),
  quest: document.getElementById('questLog'),
};

const keys = {};

const areaNames = [
  'Valle di Brinco',
  'Bosco della Pergamena',
  'Rovine di Ash',
  'Caverna della Giustizia',
  'Fortezza del Gelo',
  'Museo delle Ancore'
];

const state = {
  time: 0,
  mapLevel: 1,
  areaIndex: 0,
  gameOver: false,
  map: null,
  player: null,
  enemies: [],
  drops: [],
  particles: [],
  attackFlash: 0,
  inventory: {
    potions: 2,
    keys: 0,
    relics: 0
  },
  equipment: {
    weapon: 'Spada Arrugginita',
    armor: 'Giubba di Cuoio',
    shield: 'Scudo di Legno'
  },
  quest: 'Riusci a sconfiggere il guardiano del castello e trova il Sigillo del Nord.',
  bossDefeated: false,
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

function dist(a, b) {
  return Math.hypot(a.x - b.x, a.y - b.y);
}

function normalize(x, y) {
  const len = Math.hypot(x, y) || 1;
  return { x: x / len, y: y / len };
}

function drawRect(x, y, w, h, color) {
  ctx.fillStyle = color;
  ctx.fillRect(Math.round(x), Math.round(y), Math.round(w), Math.round(h));
}

function drawSprite(sprite, x, y, w = 16, h = 16, alpha = 1) {
  if (!sprite) return;
  const prev = ctx.globalAlpha;
  ctx.globalAlpha = alpha;
  ctx.drawImage(sprite, Math.round(x), Math.round(y), w, h);
  ctx.globalAlpha = prev;
}

function generateRoom(x, y, w, h) {
  return { x, y, w, h };
}

function isWallTile(x, y) {
  if (x < 0 || y < 0 || x >= state.map.width || y >= state.map.height) return true;
  return state.map.tiles[y][x] === 1;
}

function isWalkable(x, y) {
  if (x < 0 || y < 0 || x >= state.map.width || y >= state.map.height) return false;
  return state.map.tiles[y][x] !== 1;
}

function canMoveTo(entity, x, y) {
  const r = entity.radius || PLAYER_RADIUS;
  return !(
    isWallTile(x - r, y - r) ||
    isWallTile(x + r, y - r) ||
    isWallTile(x - r, y + r) ||
    isWallTile(x + r, y + r)
  );
}

function createDungeon(level = 1) {
  const tiles = Array.from({ length: MAP_H }, () => Array(MAP_W).fill(1));
  const rooms = [];

  const roomCount = 9 + level;
  for (let i = 0; i < roomCount; i++) {
    const w = 3 + Math.floor(Math.random() * 6);
    const h = 3 + Math.floor(Math.random() * 5);
    const x = 1 + Math.floor(Math.random() * (MAP_W - w - 2));
    const y = 1 + Math.floor(Math.random() * (MAP_H - h - 2));
    rooms.push(generateRoom(x, y, w, h));

    for (let yy = y; yy < y + h; yy++) {
      for (let xx = x; xx < x + w; xx++) {
        tiles[yy][xx] = 0;
      }
    }
  }

  for (let i = 1; i < rooms.length; i++) {
    const prev = rooms[i - 1];
    const curr = rooms[i];
    const x1 = Math.floor(prev.x + prev.w / 2);
    const y1 = Math.floor(prev.y + prev.h / 2);
    const x2 = Math.floor(curr.x + curr.w / 2);
    const y2 = Math.floor(curr.y + curr.h / 2);

    for (let x = Math.min(x1, x2); x <= Math.max(x1, x2); x++) tiles[y1][x] = 0;
    for (let y = Math.min(y1, y2); y <= Math.max(y1, y2); y++) tiles[y][x2] = 0;
  }

  const startRoom = rooms[0];
  const endRoom = rooms[rooms.length - 1];
  const start = {
    x: startRoom.x + Math.floor(startRoom.w / 2) + 0.5,
    y: startRoom.y + Math.floor(startRoom.h / 2) + 0.5,
  };
  const stairs = {
    x: endRoom.x + Math.floor(endRoom.w / 2) + 0.5,
    y: endRoom.y + Math.floor(endRoom.h / 2) + 0.5,
  };

  tiles[Math.floor(stairs.y)][Math.floor(stairs.x)] = 2;
  return {
    width: MAP_W,
    height: MAP_H,
    tiles,
    start,
    stairs,
  };
}

function createPlayer() {
  return {
    x: state.map.start.x,
    y: state.map.start.y,
    radius: PLAYER_RADIUS,
    hp: 100,
    maxHp: 100,
    speed: 2.5,
    damage: 10,
    level: 1,
    xp: 0,
    nextXp: 10,
    gold: 0,
    weaponName: state.equipment.weapon,
    attackCooldown: 0,
    hitFlash: 0,
    lastMove: { x: 1, y: 0 },
    potions: state.inventory.potions,
  };
}

function createEnemy(x, y, level) {
  const pool = ['goblin', 'orc', 'skeleton'];
  const type = choose(pool);
  const boss = level > 2 && Math.random() < 0.2;
  const base = {
    goblin: { hp: 22 + level * 5, damage: 7 + level, speed: 1.0 + level * 0.1 },
    orc: { hp: 34 + level * 7, damage: 9 + level, speed: 0.9 + level * 0.12 },
    skeleton: { hp: 28 + level * 6, damage: 9 + level, speed: 1.1 + level * 0.15 },
  }[type];

  const stats = boss ? {
    hp: 80 + level * 15,
    damage: 15 + level * 2,
    speed: 1.3 + level * 0.08,
  } : base;

  return {
    x,
    y,
    radius: boss ? 0.42 : 0.35,
    type: boss ? 'boss' : type,
    hp: stats.hp,
    maxHp: stats.hp,
    damage: stats.damage,
    speed: stats.speed,
    xp: boss ? 35 + level * 10 : 6 + level * 2,
    attackCooldown: 0,
    alive: true,
    boss,
  };
}

function spawnEnemies(level) {
  const enemies = [];
  const total = 6 + level * 2;
  let placed = 0;

  while (placed < total) {
    const x = rand(2, MAP_W - 2);
    const y = rand(2, MAP_H - 2);
    if (!isWalkable(x, y) || dist({ x, y }, state.map.start) < 4) continue;
    enemies.push(createEnemy(x + 0.5, y + 0.5, level));
    placed++;
  }

  if (level % 3 === 0 && !state.bossDefeated) {
    const bossSpotX = rand(2, MAP_W - 2);
    const bossSpotY = rand(2, MAP_H - 2);
    if (isWalkable(bossSpotX, bossSpotY) && dist({ x: bossSpotX, y: bossSpotY }, state.map.start) > 4) {
      enemies.push(createEnemy(bossSpotX + 0.5, bossSpotY + 0.5, level));
    }
  }

  state.enemies = enemies;
}

function spawnDrops() {
  const drops = [];
  for (let i = 0; i < 7; i++) {
    const x = rand(2, MAP_W - 2);
    const y = rand(2, MAP_H - 2);
    if (!isWalkable(x, y) || dist({ x, y }, state.map.start) < 2) continue;

    const roll = Math.random();
    if (roll < 0.45) drops.push({ x: x + 0.5, y: y + 0.5, type: 'gold', amount: 12 + Math.floor(Math.random() * 20), color: '#f7d36f' });
    else if (roll < 0.75) drops.push({ x: x + 0.5, y: y + 0.5, type: 'potion', amount: 1, color: '#7ad9ff' });
    else drops.push({ x: x + 0.5, y: y + 0.5, type: 'weapon', amount: 1, color: '#e7af68' });
  }
  state.drops = drops;
}

function createBurst(x, y, color, count = 8) {
  for (let i = 0; i < count; i++) {
    state.particles.push({
      x,
      y,
      dx: rand(-0.12, 0.12),
      dy: rand(-0.12, 0.12),
      life: rand(0.25, 0.8),
      color,
      size: rand(1.5, 3.5)
    });
  }
}

function gainXp(amount) {
  const p = state.player;
  p.xp += amount;
  while (p.xp >= p.nextXp) {
    p.xp -= p.nextXp;
    p.level += 1;
    p.nextXp = Math.floor(p.nextXp * 1.45) + 4;
    p.maxHp += 12;
    p.hp = p.maxHp;
    p.damage += 2;
    p.potions += 1;
  }
}

function usePotion() {
  const p = state.player;
  if (p.potions <= 0 || p.hp >= p.maxHp || state.gameOver) return;
  p.potions -= 1;
  state.inventory.potions = p.potions;
  p.hp = Math.min(p.maxHp, p.hp + 30);
  createBurst(p.x, p.y, '#7ad9ff', 12);
  updateInventoryUI();
}

function equipBySlot(slotKey, name, bonus) {
  const slotNames = {
    weapon: 'Arma',
    armor: 'Armatura',
    shield: 'Scudo'
  };

  state.equipment[slotKey] = name;
  if (slotKey === 'weapon') state.player.weaponName = name;
  if (slotKey === 'armor') state.player.maxHp += bonus;
  if (slotKey === 'shield') state.player.damage += bonus;
  updateEquipmentUI();
}

function performAttack() {
  if (state.player.attackCooldown > 0 || state.gameOver) return;
  const p = state.player;
  const dir = { x: p.lastMove.x || 1, y: p.lastMove.y || 0 };
  p.attackCooldown = 0.35;
  state.attackFlash = 0.12;

  const hitRange = 1.15;

  for (const enemy of state.enemies) {
    if (!enemy.alive) continue;
    const dx = enemy.x - p.x;
    const dy = enemy.y - p.y;
    const distToEnemy = Math.hypot(dx, dy);
    if (distToEnemy > hitRange) continue;

    const dot = dx * dir.x + dy * dir.y;
    const facing = dot >= 0 || distToEnemy < 0.5;
    if (!facing) continue;

    enemy.hp -= p.damage;
    createBurst(enemy.x, enemy.y, '#e9b96a', 10);
    if (enemy.hp <= 0) {
      enemy.alive = false;
      gainXp(enemy.xp);
      if (enemy.type === 'boss') {
        state.bossDefeated = true;
        state.inventory.relics += 1;
        state.quest = 'Il Sigillo del Nord è stato recuperato. Torna alla scala per continuare.';
        state.drops.push({ x: enemy.x, y: enemy.y, type: 'weapon', amount: 1, color: '#e7af68' });
      }
      if (Math.random() < 0.35) {
        state.drops.push({ x: enemy.x, y: enemy.y, type: 'gold', amount: 10 + Math.floor(Math.random() * 8), color: '#f7d36f' });
      }
    }
  }
}

function collectDrops() {
  const p = state.player;
  for (let i = state.drops.length - 1; i >= 0; i--) {
    const d = state.drops[i];
    if (dist(p, d) < 0.7) {
      if (d.type === 'gold') p.gold += d.amount;
      if (d.type === 'potion') {
        state.inventory.potions += 1;
        p.potions = state.inventory.potions;
      }
      if (d.type === 'weapon') {
        const options = [
          { name: 'Spada del Manuscritto', bonus: 5 },
          { name: 'Ascia del Guardiano', bonus: 7 },
          { name: 'Lancia delle Nevi', bonus: 4 }
        ];
        const picked = choose(options);
        state.equipment.weapon = picked.name;
        p.weaponName = picked.name;
        p.damage += picked.bonus;
      }
      state.drops.splice(i, 1);
      createBurst(d.x, d.y, '#f8e7a1', 10);
      updateInventoryUI();
      updateEquipmentUI();
    }
  }
}

function stepEnemy(enemy, dt) {
  if (!enemy.alive) return;
  const dx = state.player.x - enemy.x;
  const dy = state.player.y - enemy.y;
  const len = Math.hypot(dx, dy) || 1;

  if (len > 0.2) {
    const nx = dx / len;
    const ny = dy / len;
    const nextX = enemy.x + nx * enemy.speed * dt;
    const nextY = enemy.y + ny * enemy.speed * dt;

    if (canMoveTo(enemy, nextX, enemy.y)) enemy.x = nextX;
    if (canMoveTo(enemy, enemy.x, nextY)) enemy.y = nextY;
  }

  if (len < (enemy.boss ? 1.1 : 0.9) && enemy.attackCooldown <= 0) {
    state.player.hp -= enemy.damage;
    state.player.hitFlash = 0.2;
    enemy.attackCooldown = enemy.boss ? 0.8 : 1.0;
    createBurst(state.player.x, state.player.y, '#ff7d7d', enemy.boss ? 20 : 14);
    if (state.player.hp <= 0) {
      state.gameOver = true;
    }
  }
  enemy.attackCooldown = Math.max(0, enemy.attackCooldown - dt);
}

function advanceLevel() {
  if (state.gameOver) return;
  state.mapLevel += 1;
  state.areaIndex = (state.areaIndex + 1) % areaNames.length;
  state.map = createDungeon(state.mapLevel);
  state.player.x = state.map.start.x;
  state.player.y = state.map.start.y;
  state.player.hp = Math.min(state.player.maxHp, state.player.hp + 18);
  spawnEnemies(state.mapLevel);
  spawnDrops();
  ui.area.textContent = areaNames[state.areaIndex];

  if (state.mapLevel >= 3) {
    state.quest = 'Un guardiano si nasconde nel forte. Distruggi il boss per riacquistare il Sigillo.';
  }
}

function checkStairs() {
  if (dist(state.player, state.map.stairs) < 0.8) {
    advanceLevel();
  }
}

function updateInventoryUI() {
  if (!ui.inventory) return;
  const items = [
    `Pozioni: ${state.inventory.potions}`,
    `Monete: ${state.player.gold}`,
    `Reliquie: ${state.inventory.relics}`,
    `Chiavi: ${state.inventory.keys}`
  ];
  ui.inventory.innerHTML = items.map(item => `<li>${item}</li>`).join('');
}

function updateEquipmentUI() {
  if (!ui.equipment) return;
  const eq = [
    `Arma: ${state.equipment.weapon}`,
    `Armatura: ${state.equipment.armor}`,
    `Scudo: ${state.equipment.shield}`
  ];
  ui.equipment.innerHTML = eq.map(item => `<li>${item}</li>`).join('');
  ui.weapon.textContent = state.equipment.weapon;
}

function updateQuestUI() {
  if (ui.quest) ui.quest.textContent = state.quest;
}

function updateUI() {
  const p = state.player;
  ui.hp.textContent = `${Math.max(0, Math.floor(p.hp))} / ${p.maxHp}`;
  ui.level.textContent = String(p.level);
  ui.xp.textContent = `${p.xp} / ${p.nextXp}`;
  ui.gold.textContent = String(p.gold);
  ui.weapon.textContent = p.weaponName;
  updateInventoryUI();
  updateEquipmentUI();
  updateQuestUI();
}

function update(dt) {
  if (state.gameOver) return;

  state.time += dt;
  const p = state.player;
  p.attackCooldown = Math.max(0, p.attackCooldown - dt);
  p.hitFlash = Math.max(0, p.hitFlash - dt);
  state.attackFlash = Math.max(0, state.attackFlash - dt);

  let dx = 0;
  let dy = 0;

  if (keys['KeyW'] || keys['ArrowUp']) dy -= 1;
  if (keys['KeyS'] || keys['ArrowDown']) dy += 1;
  if (keys['KeyA'] || keys['ArrowLeft']) dx -= 1;
  if (keys['KeyD'] || keys['ArrowRight']) dx += 1;

  if (dx !== 0 || dy !== 0) {
    const n = normalize(dx, dy);
    const nextX = p.x + n.x * p.speed * dt;
    const nextY = p.y + n.y * p.speed * dt;

    if (canMoveTo(p, nextX, p.y)) p.x = nextX;
    if (canMoveTo(p, p.x, nextY)) p.y = nextY;
    p.lastMove.x = n.x;
    p.lastMove.y = n.y;
  }

  for (const enemy of state.enemies) {
    if (enemy.alive) stepEnemy(enemy, dt);
  }

  collectDrops();
  checkStairs();

  for (let i = state.particles.length - 1; i >= 0; i--) {
    const part = state.particles[i];
    part.x += part.dx;
    part.y += part.dy;
    part.life -= dt;
    if (part.life <= 0) state.particles.splice(i, 1);
  }

  updateUI();
}

function drawTile(tileX, tileY, screenX, screenY) {
  const tile = state.map.tiles[tileY][tileX];

  if (tile === 1) {
    const sprite = ASSETS && ASSETS.tiles && ASSETS.tiles.wall ? ASSETS.tiles.wall : null;
    if (sprite) drawSprite(sprite, screenX, screenY, TILE, TILE);
    else drawRect(screenX, screenY, TILE, TILE, '#3a2d26');
    return;
  }

  const floorSprite = ASSETS && ASSETS.tiles && ASSETS.tiles.floor ? ASSETS.tiles.floor : null;
  if (floorSprite) drawSprite(floorSprite, screenX, screenY, TILE, TILE);
  else drawRect(screenX, screenY, TILE, TILE, '#453631');

  if (tile === 2) {
    const stairs = ASSETS && ASSETS.tiles && ASSETS.tiles.stairs ? ASSETS.tiles.stairs : null;
    if (stairs) drawSprite(stairs, screenX, screenY, TILE, TILE);
    else drawRect(screenX + 4, screenY + 4, 8, 8, '#f5d76e');
  }
}

function drawWorld() {
  const cameraX = Math.floor((state.player.x * TILE) - canvas.width / 2);
  const cameraY = Math.floor((state.player.y * TILE) - canvas.height / 2);

  for (let y = 0; y < state.map.height; y++) {
    for (let x = 0; x < state.map.width; x++) {
      const screenX = x * TILE - cameraX;
      const screenY = y * TILE - cameraY;
      if (screenX < -TILE || screenX > canvas.width || screenY < -TILE || screenY > canvas.height) continue;
      drawTile(x, y, screenX, screenY);
    }
  }
}

function drawEntity(entity, sprite, size = 16) {
  const cx = entity.x * TILE - (state.player.x * TILE - canvas.width / 2);
  const cy = entity.y * TILE - (state.player.y * TILE - canvas.height / 2);

  if (sprite) {
    drawSprite(sprite, cx - size / 2, cy - size / 2, size, size, 1);
  } else {
    drawRect(cx - size / 2, cy - size / 2, size, size, '#fff');
  }
}

function drawDrops() {
  for (const drop of state.drops) {
    const cx = drop.x * TILE - (state.player.x * TILE - canvas.width / 2);
    const cy = drop.y * TILE - (state.player.y * TILE - canvas.height / 2);
    const sprite = (() => {
      if (!ASSETS || !ASSETS.items) return null;
      if (drop.type === 'gold') return ASSETS.items.gold;
      if (drop.type === 'potion') return ASSETS.items.potion;
      return ASSETS.items.sword;
    })();

    if (sprite) drawSprite(sprite, cx - 7, cy - 7, 14, 14);
    else drawRect(cx - 4, cy - 4, 8, 8, drop.color);
  }
}

function drawParticles() {
  for (const part of state.particles) {
    const x = part.x * TILE - (state.player.x * TILE - canvas.width / 2);
    const y = part.y * TILE - (state.player.y * TILE - canvas.height / 2);
    const sprite = ASSETS && ASSETS.particles && ASSETS.particles.sparkle ? ASSETS.particles.sparkle : null;
    if (sprite) drawSprite(sprite, x, y, part.size * 2, part.size * 2, Math.min(1, part.life * 2));
    else drawRect(x, y, part.size, part.size, part.color);
  }
}

function drawPlayer() {
  const p = state.player;
  const dir = p.lastMove.x < 0 ? 'left' : p.lastMove.x > 0 ? 'right' : p.lastMove.y < 0 ? 'up' : 'down';
  const sprite = ASSETS && ASSETS.playerSprites && ASSETS.playerSprites[dir] ? ASSETS.playerSprites[dir] : null;
  const cx = canvas.width / 2;
  const cy = canvas.height / 2;

  if (sprite) {
    drawSprite(sprite, cx - 8, cy - 8, 16, 16, 1);
  } else {
    drawRect(cx - 7, cy - 7, 14, 14, '#73d59f');
  }

  if (state.attackFlash > 0) {
    const beamX = cx + p.lastMove.x * 14;
    const beamY = cy + p.lastMove.y * 14;
    drawRect(beamX - 4, beamY - 4, 8, 8, '#f5d76e');
  }

  drawRect(cx - 20, cy - 18, 40, 4, 'rgba(15,15,15,0.6)');
  const hpRatio = clamp(p.hp / p.maxHp, 0, 1);
  drawRect(cx - 19, cy - 17, 38 * hpRatio, 2, '#ff7a7a');
}

function drawEnemies() {
  for (const enemy of state.enemies) {
    if (!enemy.alive) continue;
    let sprite = null;
    const mapping = {
      goblin: ASSETS && ASSETS.goblin,
      orc: ASSETS && ASSETS.orc,
      skeleton: ASSETS && ASSETS.skeleton,
      boss: ASSETS && ASSETS.orc
    };
    sprite = mapping[enemy.type] || null;
    drawEntity(enemy, sprite, enemy.boss ? 22 : 16);

    const cx = enemy.x * TILE - (state.player.x * TILE - canvas.width / 2);
    const cy = enemy.y * TILE - (state.player.y * TILE - canvas.height / 2);
    drawRect(cx - (enemy.boss ? 12 : 7), cy - (enemy.boss ? 16 : 11), enemy.boss ? 24 : 14, 2, 'rgba(15,15,15,0.7)');
    drawRect(cx - (enemy.boss ? 11 : 6), cy - (enemy.boss ? 15 : 10), (enemy.boss ? 22 : 12) * (enemy.hp / enemy.maxHp), 1, '#ff7060');
  }
}

function drawHUDBox() {
  drawRect(canvas.width / 2 - 120, 18, 240, 18, 'rgba(20,17,12,0.5)');
  drawRect(canvas.width / 2 - 118, 20, 236 * (state.player.hp / state.player.maxHp), 14, '#ff7d7d');

  if (state.gameOver) {
    drawRect(canvas.width / 2 - 170, canvas.height / 2 - 60, 340, 120, 'rgba(12,10,13,0.75)');
    ctx.fillStyle = '#f5d76e';
    ctx.font = 'bold 30px Trebuchet MS';
    ctx.textAlign = 'center';
    ctx.fillText('FALLITO', canvas.width / 2, canvas.height / 2 - 12);
    ctx.font = '18px Trebuchet MS';
    ctx.fillText('Premi R per ricominciare', canvas.width / 2, canvas.height / 2 + 26);
  }
}

function render() {
  ctx.clearRect(0, 0, canvas.width, canvas.height);
  drawWorld();
  drawDrops();
  drawParticles();
  drawEnemies();
  drawPlayer();
  drawHUDBox();
}

function resetGame() {
  state.mapLevel = 1;
  state.areaIndex = 0;
  state.map = createDungeon(1);
  state.player = createPlayer();
  state.enemies = [];
  state.drops = [];
  state.particles = [];
  state.attackFlash = 0;
  state.gameOver = false;
  state.inventory.potions = 2;
  state.inventory.keys = 0;
  state.inventory.relics = 0;
  state.equipment.weapon = 'Spada Arrugginita';
  state.equipment.armor = 'Giubba di Cuoio';
  state.equipment.shield = 'Scudo di Legno';
  state.quest = 'Riusci a sconfiggere il guardiano del castello e trova il Sigillo del Nord.';
  state.bossDefeated = false;
  ui.area.textContent = areaNames[0];
  spawnEnemies(1);
  spawnDrops();
  updateUI();
}

window.addEventListener('keydown', (event) => {
  keys[event.code] = true;
  if (event.code === 'Space') {
    event.preventDefault();
    performAttack();
  }
  if (event.code === 'KeyE') usePotion();
  if (event.code === 'KeyR' && state.gameOver) resetGame();
  if (event.code === 'Digit1') {
    state.equipment.weapon = 'Spada Arrugginita';
    state.player.weaponName = 'Spada Arrugginita';
    updateEquipmentUI();
  }
  if (event.code === 'Digit2') {
    state.equipment.weapon = 'Ascia del Guardiano';
    state.player.weaponName = 'Ascia del Guardiano';
    state.player.damage += 2;
    updateEquipmentUI();
  }
  if (event.code === 'Digit3') {
    state.equipment.weapon = 'Lancia delle Nevi';
    state.player.weaponName = 'Lancia delle Nevi';
    state.player.damage += 1;
    updateEquipmentUI();
  }
});

window.addEventListener('keyup', (event) => {
  keys[event.code] = false;
});

let last = 0;
function loop(ts) {
  const dt = Math.min((ts - last) / 1000 || 0.016, 0.033);
  last = ts;
  update(dt);
  render();
  requestAnimationFrame(loop);
}

resetGame();
requestAnimationFrame(loop);
