# Snake Game: Python → C++/Qt Conversion Plan

## 1. Dependency Mapping

| Python (pygame)               | C++ (Qt6)                                  | Notes                                          |
|-------------------------------|--------------------------------------------|-------------------------------------------------|
| `pygame.init()` / display     | `QApplication` + custom `QWidget`          | Main window with fixed 800×600                  |
| `pygame.mixer.music` (MP3)    | `QMediaPlayer` + `QAudioOutput`            | Background music looping via `setLoops(Infinite)` |
| `pygame.mixer.Sound` (WAV)    | `QSoundEffect`                             | Short SFX: eat, game_over, level_up             |
| `pygame.display.set_mode()`   | `QWidget::setFixedSize()`                  | Window sizing                                   |
| `pygame.display.set_caption()` | `QWidget::setWindowTitle()`               | Window title                                    |
| `pygame.time.Clock.tick()`    | `QTimer` with interval `1000/fps` ms       | Game loop driver                                |
| `pygame.font.SysFont`         | `QFont`                                    | Text rendering                                  |
| Surface `.render()` text      | `QPainter::drawText()` with alignment      | Centered text in `paintEvent`                   |
| `pygame.image.load()`         | `QPixmap` (from Qt Resource or file)       | Loading PNG images                              |
| `pygame.transform.scale()`    | `QPixmap::scaled()`                        | Scaling images to snake_block size              |
| `pygame.draw.rect()`          | `QPainter::fillRect()`                     | Drawing colored rectangles                      |
| `pygame.draw.circle()`        | `QPainter::drawEllipse()`                  | Drawing circles (particles)                     |
| `pygame.Surface(SRCALPHA)`    | `QPainter::setOpacity()`                   | Alpha-blended particle rendering                |
| `pygame.event.get()`          | `keyPressEvent()` / `keyReleaseEvent()`    | Keyboard input                                  |
| `pygame.key.get_pressed()`    | `QSet<int>` tracking held keys             | For speed boost on held direction key           |
| `random` module               | `QRandomGenerator::global()`               | Random food placement                           |
| `pygame.display.update()`     | `QWidget::update()` → `paintEvent()`       | Screen refresh                                  |
| Background tiling loop        | `QPainter::drawTiledPixmap()`              | Efficient native tiled background               |
| `sys.exit()` / `pygame.quit()`| `QApplication::quit()`                     | Clean shutdown                                  |

## 2. Features to Preserve (Checklist)

- [x] Difficulty selection screen (Easy 10fps / Medium 15fps / Hard 25fps)
- [x] Quit option at startup menu (Q key)
- [x] Snake movement with opposite-direction prevention
- [x] Wall collision → game over
- [x] Self collision → game over
- [x] Food eating with score increment
- [x] Snake body growth
- [x] Directional snake head images (up/down/left/right)
- [x] Body segment images
- [x] Food image
- [x] Tiled background image
- [x] Fallback to colored primitives when images unavailable
- [x] Particle effects on eating (alpha-blended circles with velocity and decay)
- [x] Background music (looping MP3)
- [x] Eat sound effect (WAV)
- [x] Game over sound effect (WAV, plays once)
- [x] Level-up sound effect (WAV, every 5 points, once per milestone)
- [x] Score display (top-left, yellow text)
- [x] Speed boost (2×) when holding current direction key
- [x] Game over screen with Play Again (C) / Quit (Q)
- [x] Window title "Snake Game by Trae"

## 3. Architecture

### Game States

```
Start → Menu ─────(1/2/3)────→ Playing ─────(collision)────→ GameOver
               │                                                  │
               ←──────────────────(C key)─────────────────────────┘
               │
          (Q key) → Quit
```

### Class Design

Single `SnakeGame` class inheriting `QWidget` with:
- **State machine**: `enum GameState { Menu, Playing, GameOver }`
- **Game loop**: `QTimer` driving `tick()` → game logic → `update()` → `paintEvent()`
- **Input**: `keyPressEvent`/`keyReleaseEvent` overrides, `QSet<int>` for held keys
- **Rendering**: `paintEvent` dispatches to `drawMenu()`, `drawGame()`, `drawGameOver()`
- **Audio**: `QMediaPlayer` (music) + `QSoundEffect` × 3 (SFX)
- **Resources**: Qt Resource System (.qrc) embedding all assets

### File Structure

```
snake_game/
├── CMakeLists.txt          # Build configuration (Qt6 Widgets + Multimedia)
├── resources.qrc           # Qt resource file referencing all assets
├── main.cpp                # Entry point: QApplication + SnakeGame widget
├── snakegame.h             # Class declaration
├── snakegame.cpp           # Full implementation
├── resources/              # (existing) images, music, sounds
│   ├── images/
│   ├── music/
│   └── sounds/
├── snake.py                # (preserved) original Python version
└── roadmap/
    └── cpp_qt_conversion_plan.md
```

## 4. Implementation Phases

### Phase 1: Project Setup
- Create `CMakeLists.txt` requiring `Qt6::Widgets` and `Qt6::Multimedia`
- Create `resources.qrc` referencing all 7 images, 1 music file, 3 sound files
- Create `main.cpp` with `QApplication` bootstrapping

### Phase 2: Core Game Widget (`snakegame.h` / `snakegame.cpp`)
- Define constants: dimensions (800×600), snake block (10), colors
- Define `Particle` struct with position, velocity, lifetime, color, size
- Define `GameState` enum
- Declare all member variables: snake list, food position, score, timers, images, audio
- Constructor: load resources, set up audio, set up timer, initialize to Menu state

### Phase 3: Input Handling
- `keyPressEvent`: route by game state
  - Menu: handle 1/2/3 (difficulty) and Q (quit)
  - Playing: handle arrow keys with opposite-direction guard
  - GameOver: handle C (restart → Menu) and Q (quit)
- `keyReleaseEvent`: remove key from held-keys set (filter auto-repeat)
- Focus policy: `Qt::StrongFocus`

### Phase 4: Game Logic (`tick()` method)
- Move snake head by current direction
- Check wall collision
- Check self collision
- Handle food eating: play sound, spawn particles, grow snake, place new food
- Handle level-up sound every 5 points
- Speed boost: if direction key held, use half the timer interval for next tick
- Remove dead particles

### Phase 5: Rendering (`paintEvent` dispatcher)
- **Menu**: black background, centered text lines with difficulty options + quit
- **Playing**:
  - Background: tiled pixmap or solid black
  - Food: pixmap or red rectangle
  - Snake body: pixmap or green rectangles
  - Snake head: directional pixmap or bright green rectangle
  - Particles: semi-transparent circles with fading alpha
  - Score: yellow text at top-left
- **GameOver**: game scene + centered red message overlay

### Phase 6: Audio Setup
- `QMediaPlayer` with `QAudioOutput` for background_music.mp3, `setLoops(Infinite)`
- Three `QSoundEffect` instances for eat.wav, game_over.wav, level_up.wav
- All audio setup wrapped in error handling (null checks, isLoaded checks)
- Game over sound plays once per game-over event

### Phase 7: Build & Test
- Install required system packages: `qt6-base-dev qt6-multimedia-dev cmake g++`
- Configure: `cmake -B build`
- Build: `cmake --build build`
- Run: `./build/snake_game`

## 5. Key Design Decisions

1. **No recursion for restart**: Python version recursively calls `gameLoop()` on restart. C++ version resets state and returns to Menu — no stack growth.

2. **Timer-based game loop**: Instead of Python's `while True` + `clock.tick()`, use `QTimer::timeout` → `tick()` → `QWidget::update()`. Timer interval adjusts for difficulty and speed boost.

3. **Qt Resource System**: All assets embedded in binary via `.qrc` for portability. Accessed via `":/path"` prefix.

4. **Efficient particle rendering**: Python creates a new `Surface` per particle per frame. C++ uses `QPainter::setOpacity()` + `drawEllipse()` directly — significantly more efficient.

5. **Held-key tracking**: Use `QSet<int>` updated on `keyPressEvent`/`keyReleaseEvent` (filtering `isAutoRepeat()`). Checked each tick for speed boost.

6. **Graceful fallback**: Images and audio load in try/catch-like patterns (check `isNull()`). Game remains fully playable without assets.

## 6. Reflection Log

### Round 1
- Confirmed `QSoundEffect` (WAV) for SFX, `QMediaPlayer` (MP3) for music
- Timer interval = `1000 / speed` ms maps correctly from pygame `clock.tick(speed)`
- Recursive gameLoop → state-reset approach eliminates stack overflow risk
- Particles: `QPainter::setOpacity()` + `drawEllipse()` replaces per-frame Surface allocation

### Round 2
- Qt6 requires CMake 3.16+
- Module requirements: `Qt6::Widgets` + `Qt6::Multimedia` (covers QSoundEffect)
- Grid alignment for food: `QRandomGenerator::global()->bounded(gridCount) * SNAKE_BLOCK`
- `keyPressEvent` auto-repeats must be filtered via `event->isAutoRepeat()`
- Background music needs `QMediaPlayer::setLoops(QMediaPlayer::Infinite)`

### Round 3
- Background tile: Python code tiles at 40px intervals using the loaded image → use `drawTiledPixmap()` natively in Qt
- `setFocusPolicy(Qt::StrongFocus)` required for QWidget to receive keyboard events
- Enable `QPainter::Antialiasing` for smooth particle circles
- Close event should stop timers and audio cleanly
- Speed boost: dynamically adjust QTimer interval at end of each tick

### Round 4 (Final)
- Verified all 20 features map cleanly to Qt equivalents
- Confirmed no feature will be lost in translation
- Code organization (4 files + .qrc) balances simplicity and separation
- Error handling strategy covers all resource loading paths
- Plan is complete and ready for implementation
