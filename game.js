* {
  box-sizing: border-box;
}

:root {
  --bg-1: #120f17;
  --bg-2: #18131d;
  --panel: rgba(28, 22, 17, 0.9);
  --gold: #f5d76e;
  --gold-deep: #b8860b;
  --parchment: #f2e4bc;
  --ink: #2b1d13;
  --stone: #6f5d4c;
  --red: #c75050;
  --green: #6fbf91;
  --blue: #8ad7ff;
}

html, body {
  margin: 0;
  min-height: 100%;
  background:
    radial-gradient(circle at top, #2a1d2b 0%, #18131d 28%, #0d0c10 100%);
  color: var(--parchment);
  font-family: "Trebuchet MS", "Segoe UI", sans-serif;
}

body {
  display: grid;
  place-items: center;
  padding: 24px;
}

.game-shell {
  width: min(100%, 1200px);
  display: flex;
  flex-direction: column;
  gap: 12px;
}

.hud {
  display: flex;
  justify-content: space-between;
  align-items: center;
  gap: 16px;
  background: linear-gradient(180deg, rgba(34, 27, 20, 0.94), rgba(18, 13, 12, 0.95));
  border: 3px solid rgba(245, 215, 110, 0.8);
  box-shadow: 0 0 0 3px rgba(0, 0, 0, 0.2), 0 8px 20px rgba(0, 0, 0, 0.35);
  padding: 12px 18px;
}

.title-panel {
  display: flex;
  flex-direction: column;
  gap: 4px;
}

.title-panel h1 {
  margin: 0;
  color: var(--gold);
  letter-spacing: 0.14em;
  font-size: clamp(1.15rem, 2.1vw, 2rem);
  text-shadow: 2px 2px 0 rgba(68, 41, 10, 0.9);
}

#areaName {
  color: #9fd8ff;
  letter-spacing: 0.14em;
  text-transform: uppercase;
  font-size: 0.72rem;
}

.stats {
  display: flex;
  flex-wrap: wrap;
  justify-content: flex-end;
  gap: 12px 18px;
  color: var(--parchment);
  font-size: 0.82rem;
  letter-spacing: 0.05em;
}

.stats strong {
  color: var(--gold);
}

.game-layout {
  display: grid;
  grid-template-columns: minmax(0, 1fr) 280px;
  gap: 12px;
}

canvas {
  display: block;
  width: 100%;
  height: auto;
  background: #111318;
  border: 4px solid #533d20;
  box-shadow: 0 0 0 4px rgba(0,0,0,0.5), 0 18px 36px rgba(0,0,0,0.5);
  image-rendering: pixelated;
  image-rendering: crisp-edges;
}

.side-panel {
  display: flex;
  flex-direction: column;
  gap: 12px;
}

.panel-block {
  background: linear-gradient(180deg, rgba(30, 24, 18, 0.96), rgba(18, 12, 12, 0.96));
  border: 2px solid rgba(245, 215, 110, 0.7);
  padding: 12px 12px 10px;
  box-shadow: inset 0 0 0 1px rgba(255,255,255,0.05);
}

.panel-block h2 {
  margin: 0 0 10px 0;
  color: var(--gold);
  font-size: 0.9rem;
  letter-spacing: 0.12em;
  text-transform: uppercase;
}

#inventoryList,
#equipmentList {
  list-style: none;
  margin: 0;
  padding: 0;
  display: flex;
  flex-direction: column;
  gap: 6px;
}

#inventoryList li,
#equipmentList li {
  background: rgba(255,255,255,0.03);
  border-left: 2px solid var(--gold);
  padding: 6px 8px;
  font-size: 0.8rem;
  color: var(--parchment);
}

#questLog {
  margin: 0;
  font-size: 0.8rem;
  line-height: 1.45;
  color: var(--blue);
}

.legend {
  display: flex;
  flex-wrap: wrap;
  justify-content: center;
  gap: 18px;
  padding: 10px 14px;
  background: rgba(10, 9, 12, 0.9);
  border: 2px solid rgba(160, 142, 100, 0.5);
  color: var(--parchment);
  font-size: 0.76rem;
  letter-spacing: 0.08em;
  text-transform: uppercase;
}

@media (max-width: 900px) {
  .game-layout {
    grid-template-columns: 1fr;
  }
}

@media (max-width: 700px) {
  .hud {
    flex-direction: column;
    align-items: flex-start;
  }

  .stats {
    justify-content: flex-start;
  }
}
