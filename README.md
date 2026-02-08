# Snake Game by Trae

A classic Snake game implementation in Python using Pygame, featuring multiple difficulty levels, acceleration mechanics, rich visual effects, and immersive sound effects.

## Features

- **Classic Gameplay**: Eat food to grow longer and avoid hitting walls or yourself.
- **Difficulty Selection**: Choose from Easy, Medium, or Hard speeds at the start.
- **Turbo Boost**: Hold down the arrow key of your current direction to double your speed.
- **Smart Controls**: Prevents accidental 180-degree turns that would cause instant death.
- **Rich Visual Effects**:
  - Custom sprite graphics for snake (head with eyes that rotates based on direction)
  - Body segments with gradient effects
  - Food items rendered as apples with highlights
  - Subtle grid background for better depth perception
  - Particle effects when eating food
- **Immersive Sound Effects**:
  - Background music that loops continuously
  - Eating sound effect when collecting food
  - Game over sound when collision occurs
  - Level up sound every 5 points
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
-   `resources/`: Directory containing all game assets:
    -   `images/`: Snake sprites, food images, and background tiles
    -   `sounds/`: Sound effects (eat, game over, level up)
    -   `music/`: Background music file

## Game Screenshots

![Snake Game Screenshot](https://github.com/user-attachments/assets/c87b6582-0999-44f6-9326-fcacf85ba20d)

*The enhanced snake game with sprite graphics, particle effects, and visual improvements*

## Build Executable (Windows)

To convert the game into a standalone `.exe` file that can run on Windows without Python installed:

1.  Ensure you have installed the requirements:
    ```bash
    pip install -r requirements.txt
    ```

2.  Run the build command:
    ```bash
    pyinstaller --onefile --noconsole --add-data "resources;resources" snake.py
    ```

3.  The executable will be generated in the `dist/` folder as `snake.exe`.

**Note**: The `--add-data` parameter has been updated to include the entire `resources` folder with all visual and sound assets.

## Technical Details

### Visual Assets
All visual assets are procedurally generated using Python's PIL (Pillow) library and stored in the `resources/images/` directory:
- Snake head sprites (4 directional variants with eyes)
- Snake body segments with gradient shading
- Food items (apple with highlights and stem)
- Background grid tiles for depth

### Sound Assets
Sound effects are generated using NumPy with synthesized waveforms and stored in the `resources/sounds/` directory:
- Eating sound: Short chirp with exponential decay
- Game over sound: Descending tone effect
- Level up sound: Ascending harmonic progression

All assets are lightweight and optimized for game performance.

Enjoy the game!
