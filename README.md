# Snake Game by Trae

A classic Snake game implementation in Python using Pygame, featuring multiple difficulty levels, acceleration mechanics, and background music.

## Features

- **Classic Gameplay**: Eat food to grow longer and avoid hitting walls or yourself.
- **Difficulty Selection**: Choose from Easy, Medium, or Hard speeds at the start.
- **Turbo Boost**: Hold down the arrow key of your current direction to double your speed.
- **Smart Controls**: Prevents accidental 180-degree turns that would cause instant death.
- **Background Music**: Enjoy retro-style background music while playing.
- **Score Tracking**: Real-time score display.

## Prerequisites

- Python 3.x installed on your system.

## Installation

1.  Clone or download this repository.
2.  Install the required dependencies:

    ```bash
    pip install -r requirements.txt
    ```

## How to Play

1.  Run the game:

    ```bash
    python snake.py
    ```

2.  **Start Screen**: Press `1`, `2`, or `3` to select your difficulty level.
    -   `1`: Easy
    -   `2`: Medium
    -   `3`: Hard

3.  **Controls**:
    -   **Arrow Keys**: Move Up, Down, Left, or Right.
    -   **Hold Arrow Key**: Accelerate (Turbo Boost) in the current direction.
    -   **Q**: Quit the game (when game over).
    -   **C**: Play again (when game over).

## Files

-   `snake.py`: The main game source code.
-   `requirements.txt`: Python dependencies.
-   `background_music.mp3`: Game background audio.

## Build Executable (Windows)

To convert the game into a standalone `.exe` file that can run on Windows without Python installed:

1.  Ensure you have installed the requirements:
    ```bash
    pip install -r requirements.txt
    ```

2.  Run the build command:
    ```bash
    pyinstaller --onefile --noconsole --add-data "background_music.mp3;." snake.py
    ```

3.  The executable will be generated in the `dist/` folder as `snake.exe`.

Enjoy the game!
