#ifndef SNAKEGAME_H
#define SNAKEGAME_H

#include <QWidget>
#include <QTimer>
#include <QPixmap>
#include <QList>
#include <QPoint>
#include <QSet>
#include <QFont>
#include <QColor>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QSoundEffect>
#include <QRandomGenerator>

class SnakeGame : public QWidget {
    Q_OBJECT

public:
    explicit SnakeGame(QWidget *parent = nullptr);
    ~SnakeGame() override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    // --- Constants ---
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;
    static constexpr int SNAKE_BLOCK = 10;

    // --- Game states ---
    enum GameState { Menu, Playing, GameOver };
    GameState m_state = Menu;

    // --- Colors ---
    const QColor WHITE{255, 255, 255};
    const QColor YELLOW{255, 255, 102};
    const QColor BLACK{0, 0, 0};
    const QColor RED{213, 50, 80};
    const QColor GREEN{0, 255, 0};
    const QColor BLUE{50, 153, 213};

    // --- Game data ---
    QList<QPoint> m_snake;
    int m_xChange = 0;
    int m_yChange = 0;
    int m_targetLength = 1;
    QPoint m_food;
    int m_snakeSpeed = 15;
    int m_lastLevelUpScore = 0;
    bool m_gameOverSoundPlayed = false;

    // --- Particle system ---
    struct Particle {
        double x, y;
        double vx, vy;
        int lifetime;
        QColor color;
        double size;
    };
    QList<Particle> m_particles;

    // --- Timer ---
    QTimer *m_gameTimer = nullptr;

    // --- Images ---
    QPixmap m_headUp, m_headDown, m_headLeft, m_headRight;
    QPixmap m_bodyImg, m_foodImg, m_bgTile;
    bool m_useImages = false;

    // --- Audio ---
    QMediaPlayer *m_musicPlayer = nullptr;
    QAudioOutput *m_audioOutput = nullptr;
    QSoundEffect *m_eatSound = nullptr;
    QSoundEffect *m_gameOverSound = nullptr;
    QSoundEffect *m_levelUpSound = nullptr;

    // --- Input ---
    QSet<int> m_pressedKeys;

    // --- Fonts ---
    QFont m_fontStyle;
    QFont m_scoreFont;

    // --- Methods ---
    void loadResources();
    void setupAudio();
    void setupTimer();

    void tick();
    void startGame(int speed);
    void placeFood();

    void drawMenu(QPainter &p);
    void drawGame(QPainter &p);
    void drawGameOver(QPainter &p);
    void drawBackground(QPainter &p);
    void drawSnake(QPainter &p);
    void drawMessage(QPainter &p, const QString &msg, const QColor &color, int yDisplace = 0);
    void drawScore(QPainter &p);

    void createFoodParticles(int x, int y);
    void updateAndDrawParticles(QPainter &p);
};

#endif // SNAKEGAME_H
