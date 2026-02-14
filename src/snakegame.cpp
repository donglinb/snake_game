#include "snakegame.h"

#include <QPainter>
#include <QKeyEvent>
#include <QApplication>
#include <QUrl>
#include <cmath>

// ─── Constructor / Destructor ────────────────────────────────────────────────

SnakeGame::SnakeGame(QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(WIDTH, HEIGHT);
    setWindowTitle("Snake Game by Trae");
    setFocusPolicy(Qt::StrongFocus);

    // Fonts
    m_fontStyle = QFont("Bahnschrift", 18);
    m_scoreFont = QFont("Comic Sans MS", 24, QFont::Bold);

    loadResources();
    setupAudio();
    setupTimer();
}

SnakeGame::~SnakeGame()
{
    if (m_musicPlayer) {
        m_musicPlayer->stop();
    }
}

// ─── Resource Loading ────────────────────────────────────────────────────────

void SnakeGame::loadResources()
{
    bool allLoaded = true;

    auto load = [&](QPixmap &pm, const QString &path) {
        if (!pm.load(path)) {
            allLoaded = false;
        }
    };

    load(m_headUp,    ":/images/snake_head_up.png");
    load(m_headDown,  ":/images/snake_head_down.png");
    load(m_headLeft,  ":/images/snake_head_left.png");
    load(m_headRight, ":/images/snake_head_right.png");
    load(m_bodyImg,   ":/images/snake_body.png");
    load(m_foodImg,   ":/images/food.png");
    load(m_bgTile,    ":/images/background_tile.png");

    if (allLoaded) {
        // Scale sprites to SNAKE_BLOCK size
        m_headUp    = m_headUp.scaled(SNAKE_BLOCK, SNAKE_BLOCK, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        m_headDown  = m_headDown.scaled(SNAKE_BLOCK, SNAKE_BLOCK, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        m_headLeft  = m_headLeft.scaled(SNAKE_BLOCK, SNAKE_BLOCK, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        m_headRight = m_headRight.scaled(SNAKE_BLOCK, SNAKE_BLOCK, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        m_bodyImg   = m_bodyImg.scaled(SNAKE_BLOCK, SNAKE_BLOCK, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        m_foodImg   = m_foodImg.scaled(SNAKE_BLOCK, SNAKE_BLOCK, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        m_useImages = true;
    }
}

// ─── Audio Setup ─────────────────────────────────────────────────────────────

void SnakeGame::setupAudio()
{
    // Background music
    m_audioOutput = new QAudioOutput(this);
    m_audioOutput->setVolume(0.5);
    m_musicPlayer = new QMediaPlayer(this);
    m_musicPlayer->setAudioOutput(m_audioOutput);
    m_musicPlayer->setSource(QUrl("qrc:/music/background_music.mp3"));
    m_musicPlayer->setLoops(QMediaPlayer::Infinite);
    m_musicPlayer->play();

    // Sound effects
    auto loadEffect = [this](QSoundEffect *&effect, const QString &path) {
        effect = new QSoundEffect(this);
        effect->setSource(QUrl(path));
        effect->setVolume(0.7);
    };

    loadEffect(m_eatSound,      "qrc:/sounds/eat.wav");
    loadEffect(m_gameOverSound, "qrc:/sounds/game_over.wav");
    loadEffect(m_levelUpSound,  "qrc:/sounds/level_up.wav");
}

// ─── Timer Setup ─────────────────────────────────────────────────────────────

void SnakeGame::setupTimer()
{
    m_gameTimer = new QTimer(this);
    connect(m_gameTimer, &QTimer::timeout, this, &SnakeGame::tick);
}

// ─── Start / Reset Game ──────────────────────────────────────────────────────

void SnakeGame::startGame(int speed)
{
    m_snakeSpeed = speed;
    m_xChange = 0;
    m_yChange = 0;
    m_targetLength = 1;
    m_lastLevelUpScore = 0;
    m_gameOverSoundPlayed = false;
    m_particles.clear();
    m_pressedKeys.clear();

    m_snake.clear();
    m_snake.append(QPoint(WIDTH / 2, HEIGHT / 2));

    placeFood();

    m_state = Playing;
    m_gameTimer->start(1000 / m_snakeSpeed);
    update();
}

// ─── Food Placement ──────────────────────────────────────────────────────────

void SnakeGame::placeFood()
{
    int gridW = (WIDTH - SNAKE_BLOCK) / SNAKE_BLOCK;
    int gridH = (HEIGHT - SNAKE_BLOCK) / SNAKE_BLOCK;
    m_food.setX(QRandomGenerator::global()->bounded(gridW + 1) * SNAKE_BLOCK);
    m_food.setY(QRandomGenerator::global()->bounded(gridH + 1) * SNAKE_BLOCK);
}

// ─── Game Tick ───────────────────────────────────────────────────────────────

void SnakeGame::tick()
{
    if (m_state != Playing) return;

    // Move
    int headX = m_snake.last().x() + m_xChange;
    int headY = m_snake.last().y() + m_yChange;

    // Wall collision
    if (headX < 0 || headX >= WIDTH || headY < 0 || headY >= HEIGHT) {
        m_state = GameOver;
        m_gameTimer->stop();
        if (m_gameOverSound && !m_gameOverSoundPlayed) {
            m_gameOverSound->play();
            m_gameOverSoundPlayed = true;
        }
        update();
        return;
    }

    QPoint newHead(headX, headY);
    m_snake.append(newHead);

    // Trim tail
    while (m_snake.size() > m_targetLength) {
        m_snake.removeFirst();
    }

    // Self collision (check against all but the head)
    for (int i = 0; i < m_snake.size() - 1; ++i) {
        if (m_snake[i] == newHead) {
            m_state = GameOver;
            m_gameTimer->stop();
            if (m_gameOverSound && !m_gameOverSoundPlayed) {
                m_gameOverSound->play();
                m_gameOverSoundPlayed = true;
            }
            update();
            return;
        }
    }

    // Food collision
    if (newHead == m_food) {
        if (m_eatSound) m_eatSound->play();
        createFoodParticles(m_food.x(), m_food.y());
        m_targetLength++;
        placeFood();

        int score = m_targetLength - 1;
        if (score > 0 && score % 5 == 0 && score != m_lastLevelUpScore) {
            if (m_levelUpSound) m_levelUpSound->play();
            m_lastLevelUpScore = score;
        }
    }

    // Update particles
    for (auto &p : m_particles) {
        p.x += p.vx;
        p.y += p.vy;
        p.lifetime--;
        p.size = std::max(1.0, p.size * 0.95);
    }
    m_particles.erase(
        std::remove_if(m_particles.begin(), m_particles.end(),
                       [](const Particle &p) { return p.lifetime <= 0; }),
        m_particles.end());

    // Speed boost: if direction key is held, use double speed
    bool boosted = false;
    if ((m_xChange < 0 && m_pressedKeys.contains(Qt::Key_Left)) ||
        (m_xChange > 0 && m_pressedKeys.contains(Qt::Key_Right)) ||
        (m_yChange < 0 && m_pressedKeys.contains(Qt::Key_Up)) ||
        (m_yChange > 0 && m_pressedKeys.contains(Qt::Key_Down))) {
        boosted = true;
    }
    int effectiveSpeed = boosted ? m_snakeSpeed * 2 : m_snakeSpeed;
    m_gameTimer->setInterval(1000 / effectiveSpeed);

    update();
}

// ─── Key Events ──────────────────────────────────────────────────────────────

void SnakeGame::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) {
        QWidget::keyPressEvent(event);
        return;
    }

    m_pressedKeys.insert(event->key());

    switch (m_state) {
    case Menu:
        switch (event->key()) {
        case Qt::Key_1: startGame(10); break;   // Easy
        case Qt::Key_2: startGame(15); break;   // Medium
        case Qt::Key_3: startGame(25); break;   // Hard
        case Qt::Key_Q: QApplication::quit(); break;
        default: break;
        }
        break;

    case Playing:
        switch (event->key()) {
        case Qt::Key_Left:
            if (m_xChange != SNAKE_BLOCK) { m_xChange = -SNAKE_BLOCK; m_yChange = 0; }
            break;
        case Qt::Key_Right:
            if (m_xChange != -SNAKE_BLOCK) { m_xChange = SNAKE_BLOCK; m_yChange = 0; }
            break;
        case Qt::Key_Up:
            if (m_yChange != SNAKE_BLOCK) { m_yChange = -SNAKE_BLOCK; m_xChange = 0; }
            break;
        case Qt::Key_Down:
            if (m_yChange != -SNAKE_BLOCK) { m_yChange = SNAKE_BLOCK; m_xChange = 0; }
            break;
        default: break;
        }
        break;

    case GameOver:
        switch (event->key()) {
        case Qt::Key_C:
            m_state = Menu;
            update();
            break;
        case Qt::Key_Q:
            QApplication::quit();
            break;
        default: break;
        }
        break;
    }
}

void SnakeGame::keyReleaseEvent(QKeyEvent *event)
{
    if (!event->isAutoRepeat()) {
        m_pressedKeys.remove(event->key());
    }
    QWidget::keyReleaseEvent(event);
}

// ─── Paint Event ─────────────────────────────────────────────────────────────

void SnakeGame::paintEvent(QPaintEvent * /*event*/)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    switch (m_state) {
    case Menu:     drawMenu(p);     break;
    case Playing:  drawGame(p);     break;
    case GameOver: drawGameOver(p); break;
    }
}

// ─── Draw: Menu ──────────────────────────────────────────────────────────────

void SnakeGame::drawMenu(QPainter &p)
{
    p.fillRect(rect(), BLACK);
    drawMessage(p, "Select Difficulty:", WHITE, -75);
    drawMessage(p, "1. Easy",           GREEN, -25);
    drawMessage(p, "2. Medium",         YELLOW,  25);
    drawMessage(p, "3. Hard",           RED,     75);
    drawMessage(p, "Q. Quit",           WHITE,  125);
}

// ─── Draw: Game ──────────────────────────────────────────────────────────────

void SnakeGame::drawGame(QPainter &p)
{
    drawBackground(p);

    // Food
    if (m_useImages) {
        p.drawPixmap(m_food, m_foodImg);
    } else {
        p.fillRect(m_food.x(), m_food.y(), SNAKE_BLOCK, SNAKE_BLOCK, RED);
    }

    drawSnake(p);
    updateAndDrawParticles(p);
    drawScore(p);
}

// ─── Draw: Game Over ─────────────────────────────────────────────────────────

void SnakeGame::drawGameOver(QPainter &p)
{
    drawBackground(p);
    drawSnake(p);
    updateAndDrawParticles(p);
    drawScore(p);
    drawMessage(p, "You Lost! Press C-Play Again or Q-Quit", RED);
}

// ─── Draw Helpers ────────────────────────────────────────────────────────────

void SnakeGame::drawBackground(QPainter &p)
{
    if (m_useImages && !m_bgTile.isNull()) {
        p.drawTiledPixmap(rect(), m_bgTile);
    } else {
        p.fillRect(rect(), BLACK);
    }
}

void SnakeGame::drawSnake(QPainter &p)
{
    if (m_snake.isEmpty()) return;

    // Body segments (all except last which is the head)
    for (int i = 0; i < m_snake.size() - 1; ++i) {
        const QPoint &seg = m_snake[i];
        if (m_useImages) {
            p.drawPixmap(seg, m_bodyImg);
        } else {
            p.fillRect(seg.x(), seg.y(), SNAKE_BLOCK, SNAKE_BLOCK, GREEN);
        }
    }

    // Head
    const QPoint &head = m_snake.last();
    if (m_useImages) {
        QPixmap *headImg = &m_headRight; // default
        if (m_yChange < 0)      headImg = &m_headUp;
        else if (m_yChange > 0) headImg = &m_headDown;
        else if (m_xChange < 0) headImg = &m_headLeft;
        else                     headImg = &m_headRight;
        p.drawPixmap(head, *headImg);
    } else {
        p.fillRect(head.x(), head.y(), SNAKE_BLOCK, SNAKE_BLOCK, QColor(0, 255, 100));
    }
}

void SnakeGame::drawMessage(QPainter &p, const QString &msg, const QColor &color, int yDisplace)
{
    p.setFont(m_fontStyle);
    p.setPen(color);
    QRect textRect(0, HEIGHT / 2 + yDisplace - 15, WIDTH, 30);
    p.drawText(textRect, Qt::AlignCenter, msg);
}

void SnakeGame::drawScore(QPainter &p)
{
    p.setFont(m_scoreFont);
    p.setPen(YELLOW);
    p.drawText(10, 35, QString("Your Score: %1").arg(m_targetLength - 1));
}

// ─── Particles ───────────────────────────────────────────────────────────────

void SnakeGame::createFoodParticles(int x, int y)
{
    for (int i = 0; i < 15; ++i) {
        Particle pt;
        pt.x = x + SNAKE_BLOCK / 2.0;
        pt.y = y + SNAKE_BLOCK / 2.0;
        pt.vx = QRandomGenerator::global()->bounded(6001) / 1000.0 - 3.0; // [-3, 3]
        pt.vy = QRandomGenerator::global()->bounded(6001) / 1000.0 - 3.0;
        pt.lifetime = 30;
        pt.color = QColor(255, 200, 0);
        pt.size = QRandomGenerator::global()->bounded(2, 5); // [2, 4]
        m_particles.append(pt);
    }
}

void SnakeGame::updateAndDrawParticles(QPainter &p)
{
    for (const auto &pt : m_particles) {
        if (pt.lifetime <= 0) continue;
        double alpha = static_cast<double>(pt.lifetime) / 30.0;
        p.setOpacity(alpha);
        QColor c = pt.color;
        p.setBrush(c);
        p.setPen(Qt::NoPen);
        int sz = static_cast<int>(pt.size);
        p.drawEllipse(QPointF(pt.x, pt.y), sz, sz);
    }
    p.setOpacity(1.0);
}
