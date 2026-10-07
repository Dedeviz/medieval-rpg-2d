// ASSET GENERATOR - Medieval Manuscript Illuminated Style
// Genera sprite e tileset in stile miniature medievali con decorazioni dorate

const ASSETS = {};

// Colori palette medievale
const PALETTE = {
  gold: '#d4af37',
  goldDark: '#8b7620',
  goldLight: '#f5d76e',
  parchment: '#f4e8d0',
  parchmentDark: '#d9cdb5',
  ink: '#2a1810',
  crimson: '#8b1a1a',
  blue: '#003d82',
  blueDark: '#001f4d',
  green: '#2d5016',
  bronze: '#8b6914',
  white: '#f5f5f0',
  shadow: '#4a3728',
};

function createCanvas(w, h) {
  const canvas = document.createElement('canvas');
  canvas.width = w;
  canvas.height = h;
  return canvas;
}

function drawOnCanvas(canvas, fn) {
  const ctx = canvas.getContext('2d');
  ctx.imageSmoothingEnabled = false;
  fn(ctx);
  return canvas;
}

// === PLAYER CHARACTER - Cavaliere Medievale ===
ASSETS.playerSprites = {};

['up', 'down', 'left', 'right'].forEach(dir => {
  ASSETS.playerSprites[dir] = drawOnCanvas(createCanvas(16, 16), (ctx) => {
    // Background parchment
    ctx.fillStyle = PALETTE.parchment;
    ctx.fillRect(0, 0, 16, 16);
    
    // Decorative border
    ctx.strokeStyle = PALETTE.gold;
    ctx.lineWidth = 1;
    ctx.strokeRect(1, 1, 14, 14);

    // Helmet - gold
    ctx.fillStyle = PALETTE.gold;
    ctx.fillRect(5, 3, 6, 3);
    ctx.fillRect(4, 6, 8, 1);
    
    // Visor dark
    ctx.fillStyle = PALETTE.ink;
    ctx.fillRect(6, 5, 1, 2);
    ctx.fillRect(9, 5, 1, 2);

    // Body - armor blue
    ctx.fillStyle = PALETTE.blue;
    ctx.fillRect(5, 7, 6, 4);
    
    // Armor details
    ctx.fillStyle = PALETTE.gold;
    ctx.fillRect(5, 8, 6, 1);

    // Legs - armor
    ctx.fillStyle = PALETTE.blueDark;
    ctx.fillRect(6, 11, 4, 3);

    // Shield (left)
    if (dir === 'left') {
      ctx.fillStyle = PALETTE.crimson;
      ctx.fillRect(2, 8, 2, 3);
      ctx.fillStyle = PALETTE.gold;
      ctx.fillRect(3, 9, 1, 1);
    }
    // Sword (right)
    if (dir === 'right') {
      ctx.fillStyle = PALETTE.bronze;
      ctx.fillRect(12, 7, 2, 6);
      ctx.fillStyle = PALETTE.gold;
      ctx.fillRect(12, 6, 2, 1);
    }

    // Decorative illumination corner
    ctx.fillStyle = PALETTE.goldLight;
    ctx.fillRect(1, 1, 2, 2);
    ctx.fillRect(13, 1, 2, 2);
  });
});

// === ENEMIES - Medieval Monsters ===

// GOBLIN - verdastro, malandrino
ASSETS.goblin = drawOnCanvas(createCanvas(16, 16), (ctx) => {
  ctx.fillStyle = PALETTE.parchment;
  ctx.fillRect(0, 0, 16, 16);
  
  ctx.strokeStyle = PALETTE.goldDark;
  ctx.lineWidth = 1;
  ctx.strokeRect(1, 1, 14, 14);

  // Head - green
  ctx.fillStyle = PALETTE.green;
  ctx.fillRect(5, 3, 6, 5);
  
  // Eyes - malvagi
  ctx.fillStyle = PALETTE.ink;
  ctx.fillRect(6, 5, 1, 1);
  ctx.fillRect(9, 5, 1, 1);
  ctx.fillStyle = PALETTE.crimson;
  ctx.fillRect(6, 6, 1, 1);
  ctx.fillRect(9, 6, 1, 1);

  // Ears appuntiti
  ctx.fillStyle = PALETTE.goldDark;
  ctx.fillRect(4, 2, 1, 2);
  ctx.fillRect(11, 2, 1, 2);

  // Body
  ctx.fillStyle = PALETTE.green;
  ctx.fillRect(6, 8, 4, 3);

  // Arms
  ctx.fillStyle = PALETTE.green;
  ctx.fillRect(4, 9, 2, 2);
  ctx.fillRect(10, 9, 2, 2);

  // Arma - bastone
  ctx.fillStyle = PALETTE.bronze;
  ctx.fillRect(2, 7, 1, 4);
});

// ORC - robusto e aggressivo
ASSETS.orc = drawOnCanvas(createCanvas(16, 16), (ctx) => {
  ctx.fillStyle = PALETTE.parchment;
  ctx.fillRect(0, 0, 16, 16);
  
  ctx.strokeStyle = PALETTE.gold;
  ctx.lineWidth = 1;
  ctx.strokeRect(1, 1, 14, 14);

  // Head - marrone
  ctx.fillStyle = PALETTE.shadow;
  ctx.fillRect(5, 2, 6, 6);
  
  // Tusks - avorio
  ctx.fillStyle = PALETTE.white;
  ctx.fillRect(4, 6, 1, 2);
  ctx.fillRect(11, 6, 1, 2);

  // Eyes - rossi fuoco
  ctx.fillStyle = PALETTE.crimson;
  ctx.fillRect(6, 4, 1, 2);
  ctx.fillRect(9, 4, 1, 2);

  // Horns
  ctx.fillStyle = PALETTE.bronze;
  ctx.fillRect(5, 1, 1, 2);
  ctx.fillRect(10, 1, 1, 2);

  // Body - muscoloso
  ctx.fillStyle = PALETTE.shadow;
  ctx.fillRect(5, 8, 6, 4);

  // Arma - ascia
  ctx.fillStyle = PALETTE.bronze;
  ctx.fillRect(11, 6, 1, 5);
  ctx.fillStyle = PALETTE.gold;
  ctx.fillRect(10, 5, 3, 2);
});

// SKELETON - scheletro luminoso
ASSETS.skeleton = drawOnCanvas(createCanvas(16, 16), (ctx) => {
  ctx.fillStyle = PALETTE.parchment;
  ctx.fillRect(0, 0, 16, 16);
  
  ctx.strokeStyle = PALETTE.ink;
  ctx.lineWidth = 1;
  ctx.strokeRect(1, 1, 14, 14);

  // Skull - bianco
  ctx.fillStyle = PALETTE.white;
  ctx.fillRect(5, 2, 6, 5);

  // Eye sockets - nero luminoso
  ctx.fillStyle = PALETTE.blueDark;
  ctx.fillRect(6, 4, 1, 2);
  ctx.fillRect(9, 4, 1, 2);
  
  ctx.fillStyle = PALETTE.blue;
  ctx.fillRect(6, 4, 1, 1);
  ctx.fillRect(9, 4, 1, 1);

  // Teeth
  ctx.fillStyle = PALETTE.ink;
  ctx.fillRect(5, 6, 1, 1);
  ctx.fillRect(7, 6, 1, 1);
  ctx.fillRect(9, 6, 1, 1);
  ctx.fillRect(11, 6, 1, 1);

  // Spine - ossa
  ctx.fillStyle = PALETTE.white;
  ctx.fillRect(7, 7, 2, 2);
  ctx.fillRect(7, 9, 2, 2);
  ctx.fillRect(7, 11, 2, 2);

  // Ribs
  ctx.fillStyle = PALETTE.goldLight;
  ctx.fillRect(5, 9, 1, 2);
  ctx.fillRect(10, 9, 1, 2);

  // Arma - spada spettrale
  ctx.fillStyle = PALETTE.blue;
  ctx.fillRect(12, 7, 1, 5);
});

// === TILESET - Dungeon Medievale ===

ASSETS.tiles = {};

// FLOOR - pietra con motivi medievali
ASSETS.tiles.floor = drawOnCanvas(createCanvas(16, 16), (ctx) => {
  ctx.fillStyle = PALETTE.parchmentDark;
  ctx.fillRect(0, 0, 16, 16);

  // Motivi di pietra
  ctx.fillStyle = PALETTE.shadow;
  ctx.fillRect(0, 0, 16, 1);
  ctx.fillRect(0, 0, 1, 16);
  ctx.fillRect(15, 0, 1, 16);
  ctx.fillRect(0, 15, 16, 1);

  // Decorazioni dorate negli angoli
  ctx.fillStyle = PALETTE.gold;
  ctx.fillRect(1, 1, 2, 2);
  ctx.fillRect(13, 1, 2, 2);
  ctx.fillRect(1, 13, 2, 2);
  ctx.fillRect(13, 13, 2, 2);

  // Motivo centrale
  ctx.fillStyle = PALETTE.goldLight;
  ctx.fillRect(7, 7, 2, 2);
});

// WALL - muro medievale con texture
ASSETS.tiles.wall = drawOnCanvas(createCanvas(16, 16), (ctx) => {
  ctx.fillStyle = PALETTE.shadow;
  ctx.fillRect(0, 0, 16, 16);

  // Mattoni
  ctx.strokeStyle = PALETTE.ink;
  ctx.lineWidth = 1;
  ctx.strokeRect(1, 1, 7, 7);
  ctx.strokeRect(9, 1, 6, 7);
  ctx.strokeRect(1, 9, 7, 6);
  ctx.strokeRect(9, 9, 6, 6);

  // Highlights illuminazione medievale
  ctx.fillStyle = PALETTE.goldDark;
  ctx.fillRect(2, 2, 1, 1);
  ctx.fillRect(10, 2, 1, 1);
  ctx.fillRect(2, 10, 1, 1);
  ctx.fillRect(10, 10, 1, 1);

  // Decorazioni dorate nelle fughe
  ctx.fillStyle = PALETTE.gold;
  ctx.fillRect(0, 8, 16, 1);
  ctx.fillRect(8, 0, 1, 16);
});

// WALL ORNATE - muro decorato con disegni
ASSETS.tiles.wallOrnate = drawOnCanvas(createCanvas(16, 16), (ctx) => {
  ctx.fillStyle = PALETTE.shadow;
  ctx.fillRect(0, 0, 16, 16);

  // Base muro
  ctx.strokeStyle = PALETTE.ink;
  ctx.lineWidth = 1;
  ctx.strokeRect(1, 1, 14, 14);

  // Disegni medievali - stemmi
  ctx.fillStyle = PALETTE.crimson;
  ctx.fillRect(6, 4, 4, 4);
  ctx.fillStyle = PALETTE.gold;
  ctx.fillRect(7, 5, 2, 2);

  // Decorazioni bordo
  ctx.fillStyle = PALETTE.gold;
  ctx.fillRect(2, 1, 1, 1);
  ctx.fillRect(5, 1, 1, 1);
  ctx.fillRect(10, 1, 1, 1);
  ctx.fillRect(13, 1, 1, 1);

  // Motivi inferiori
  ctx.fillRect(2, 14, 1, 1);
  ctx.fillRect(5, 14, 1, 1);
  ctx.fillRect(10, 14, 1, 1);
  ctx.fillRect(13, 14, 1, 1);
});

// PILLAR - colonna medievale
ASSETS.tiles.pillar = drawOnCanvas(createCanvas(16, 16), (ctx) => {
  ctx.fillStyle = PALETTE.parchmentDark;
  ctx.fillRect(0, 0, 16, 16);

  // Colonna
  ctx.fillStyle = PALETTE.shadow;
  ctx.fillRect(5, 2, 6, 12);

  // Capitello dorato
  ctx.fillStyle = PALETTE.gold;
  ctx.fillRect(4, 1, 8, 2);

  // Base dorata
  ctx.fillRect(4, 13, 8, 2);

  // Decorazioni
  ctx.fillStyle = PALETTE.goldLight;
  ctx.fillRect(7, 5, 2, 1);
  ctx.fillRect(7, 9, 2, 1);
});

// STAIRS - scale per il livello successivo
ASSETS.tiles.stairs = drawOnCanvas(createCanvas(16, 16), (ctx) => {
  ctx.fillStyle = PALETTE.parchment;
  ctx.fillRect(0, 0, 16, 16);

  // Gradini dorati illuminati
  ctx.fillStyle = PALETTE.gold;
  for (let i = 0; i < 4; i++) {
    ctx.fillRect(2 + i * 3, 3 + i * 3, 3, 3);
  }

  // Luci su gradini
  ctx.fillStyle = PALETTE.goldLight;
  ctx.fillRect(3, 4, 1, 1);
  ctx.fillRect(6, 7, 1, 1);
  ctx.fillRect(9, 10, 1, 1);
  ctx.fillRect(12, 13, 1, 1);

  // Border decorativo
  ctx.strokeStyle = PALETTE.goldDark;
  ctx.lineWidth = 1;
  ctx.strokeRect(1, 1, 14, 14);
});

// TORCH - fiamma di torcia
ASSETS.tiles.torch = drawOnCanvas(createCanvas(16, 16), (ctx) => {
  ctx.fillStyle = PALETTE.parchmentDark;
  ctx.fillRect(0, 0, 16, 16);

  // Supporto di ferro
  ctx.fillStyle = PALETTE.bronze;
  ctx.fillRect(6, 8, 4, 6);

  // Fiamma - gradiente arancione/rosso
  ctx.fillStyle = PALETTE.crimson;
  ctx.fillRect(7, 2, 2, 3);
  ctx.fillStyle = '#ff8c00';
  ctx.fillRect(6, 4, 4, 3);
  ctx.fillStyle = '#ffd700';
  ctx.fillRect(7, 5, 2, 2);

  // Glow effect
  ctx.fillStyle = 'rgba(255, 200, 0, 0.3)';
  ctx.fillRect(5, 3, 6, 4);
});

// === ITEMS ===

ASSETS.items = {};

// POTION - pozione di guarigione
ASSETS.items.potion = drawOnCanvas(createCanvas(12, 12), (ctx) => {
  ctx.fillStyle = PALETTE.parchment;
  ctx.fillRect(0, 0, 12, 12);

  // Bottiglia
  ctx.fillStyle = PALETTE.blue;
  ctx.fillRect(4, 2, 4, 6);

  // Tappo dorato
  ctx.fillStyle = PALETTE.gold;
  ctx.fillRect(4, 1, 4, 1);

  // Pozione dentro - luminosa
  ctx.fillStyle = PALETTE.goldLight;
  ctx.fillRect(5, 4, 2, 3);

  // Scintille magiche
  ctx.fillStyle = PALETTE.goldLight;
  ctx.fillRect(3, 5, 1, 1);
  ctx.fillRect(8, 6, 1, 1);
});

// GOLD COIN - moneta d'oro
ASSETS.items.gold = drawOnCanvas(createCanvas(10, 10), (ctx) => {
  ctx.fillStyle = PALETTE.parchment;
  ctx.fillRect(0, 0, 10, 10);

  // Moneta circolare
  ctx.fillStyle = PALETTE.gold;
  ctx.beginPath();
  ctx.arc(5, 5, 3, 0, Math.PI * 2);
  ctx.fill();

  // Bordo scuro
  ctx.strokeStyle = PALETTE.goldDark;
  ctx.lineWidth = 1;
  ctx.beginPath();
  ctx.arc(5, 5, 3, 0, Math.PI * 2);
  ctx.stroke();

  // Simbolo centrale
  ctx.fillStyle = PALETTE.goldDark;
  ctx.fillRect(4, 4, 2, 2);

  // Rilievo
  ctx.fillStyle = PALETTE.goldLight;
  ctx.fillRect(4, 4, 1, 1);
});

// SWORD - spada
ASSETS.items.sword = drawOnCanvas(createCanvas(14, 12), (ctx) => {
  ctx.fillStyle = PALETTE.parchment;
  ctx.fillRect(0, 0, 14, 12);

  // Lama
  ctx.fillStyle = PALETTE.blue;
  ctx.fillRect(5, 1, 4, 8);

  // Highlights sulla lama
  ctx.fillStyle = PALETTE.goldLight;
  ctx.fillRect(6, 2, 1, 6);

  // Elsa
  ctx.fillStyle = PALETTE.bronze;
  ctx.fillRect(6, 9, 2, 2);

  // Pomolo dorato
  ctx.fillStyle = PALETTE.gold;
  ctx.fillRect(6, 11, 2, 1);
});

// === DECORATIVE ELEMENTS ===

ASSETS.borders = {};

// Border - cornice medievale con illuminazioni
ASSETS.borders.medieval = (canvasW, canvasH) => {
  const canvas = createCanvas(canvasW, canvasH);
  drawOnCanvas(canvas, (ctx) => {
    ctx.fillStyle = 'rgba(212, 175, 55, 0.15)';
    ctx.fillRect(0, 0, canvasW, 8);
    ctx.fillRect(0, canvasH - 8, canvasW, 8);
    ctx.fillRect(0, 0, 8, canvasH);
    ctx.fillRect(canvasW - 8, 0, 8, canvasH);

    // Angoli decorati
    ctx.fillStyle = PALETTE.gold;
    ctx.fillRect(2, 2, 4, 4);
    ctx.fillRect(canvasW - 6, 2, 4, 4);
    ctx.fillRect(2, canvasH - 6, 4, 4);
    ctx.fillRect(canvasW - 6, canvasH - 6, 4, 4);
  });
  return canvas;
};

// === EFFECTS - Particelle e effetti ===

ASSETS.particles = {};

// Scintille dorate
ASSETS.particles.sparkle = drawOnCanvas(createCanvas(4, 4), (ctx) => {
  ctx.fillStyle = PALETTE.gold;
  ctx.fillRect(1, 0, 2, 1);
  ctx.fillRect(0, 1, 4, 2);
  ctx.fillRect(1, 3, 2, 1);
  
  ctx.fillStyle = PALETTE.goldLight;
  ctx.fillRect(1, 1, 2, 2);
});

// Effetto sangue/danno
ASSETS.particles.blood = drawOnCanvas(createCanvas(4, 4), (ctx) => {
  ctx.fillStyle = PALETTE.crimson;
  ctx.fillRect(1, 0, 2, 2);
  ctx.fillRect(0, 2, 4, 2);
  
  ctx.fillStyle = '#8b1a1a';
  ctx.fillRect(1, 1, 2, 2);
});

// Effetto magia blu
ASSETS.particles.magic = drawOnCanvas(createCanvas(4, 4), (ctx) => {
  ctx.fillStyle = PALETTE.blue;
  ctx.fillRect(1, 0, 2, 1);
  ctx.fillRect(0, 1, 4, 2);
  ctx.fillRect(1, 3, 2, 1);
  
  ctx.fillStyle = PALETTE.goldLight;
  ctx.fillRect(1, 1, 1, 1);
});

// === UTILITY ===

ASSETS.toDataURL = (canvas) => canvas.toDataURL();

ASSETS.draw = (canvas, ctx, x, y, opacity = 1) => {
  const prevAlpha = ctx.globalAlpha;
  ctx.globalAlpha = opacity;
  ctx.drawImage(canvas, x, y);
  ctx.globalAlpha = prevAlpha;
};

console.log('✓ Assets loaded - Medieval manuscript style');
