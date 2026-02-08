import pygame
import time
import random
import os
import sys

def resource_path(relative_path):
    """ Get absolute path to resource, works for dev and for PyInstaller """
    try:
        # PyInstaller creates a temp folder and stores path in _MEIPASS
        base_path = sys._MEIPASS
    except Exception:
        base_path = os.path.abspath(".")

    return os.path.join(base_path, relative_path)

pygame.init()
pygame.mixer.init()

# Load and play background music
try:
    music_path = resource_path('resources/music/background_music.mp3')
    pygame.mixer.music.load(music_path)
    pygame.mixer.music.play(-1)  # -1 means loop indefinitely
except pygame.error as e:
    print(f"Could not load music: {e}")

# Load sound effects
try:
    eat_sound = pygame.mixer.Sound(resource_path('resources/sounds/eat.wav'))
    game_over_sound = pygame.mixer.Sound(resource_path('resources/sounds/game_over.wav'))
    level_up_sound = pygame.mixer.Sound(resource_path('resources/sounds/level_up.wav'))
except pygame.error as e:
    print(f"Could not load sounds: {e}")
    eat_sound = None
    game_over_sound = None
    level_up_sound = None

# Define colors
WHITE = (255, 255, 255)
YELLOW = (255, 255, 102)
BLACK = (0, 0, 0)
RED = (213, 50, 80)
GREEN = (0, 255, 0)
BLUE = (50, 153, 213)

# Display dimensions
WIDTH = 800
HEIGHT = 600

dis = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption('Snake Game by Trae')

clock = pygame.time.Clock()

snake_block = 10
snake_speed = 15

font_style = pygame.font.SysFont("bahnschrift", 25)
score_font = pygame.font.SysFont("comicsansms", 35)

# Load images
try:
    snake_head_up = pygame.image.load(resource_path('resources/images/snake_head_up.png'))
    snake_head_down = pygame.image.load(resource_path('resources/images/snake_head_down.png'))
    snake_head_left = pygame.image.load(resource_path('resources/images/snake_head_left.png'))
    snake_head_right = pygame.image.load(resource_path('resources/images/snake_head_right.png'))
    snake_body_img = pygame.image.load(resource_path('resources/images/snake_body.png'))
    food_img = pygame.image.load(resource_path('resources/images/food.png'))
    background_tile = pygame.image.load(resource_path('resources/images/background_tile.png'))
    
    # Scale images to match snake_block size
    snake_head_up = pygame.transform.scale(snake_head_up, (snake_block, snake_block))
    snake_head_down = pygame.transform.scale(snake_head_down, (snake_block, snake_block))
    snake_head_left = pygame.transform.scale(snake_head_left, (snake_block, snake_block))
    snake_head_right = pygame.transform.scale(snake_head_right, (snake_block, snake_block))
    snake_body_img = pygame.transform.scale(snake_body_img, (snake_block, snake_block))
    food_img = pygame.transform.scale(food_img, (snake_block, snake_block))
    
    use_images = True
except pygame.error as e:
    print(f"Could not load images: {e}")
    use_images = False

# Particle system for effects
class Particle:
    def __init__(self, x, y, color):
        self.x = x
        self.y = y
        self.vx = random.uniform(-3, 3)
        self.vy = random.uniform(-3, 3)
        self.lifetime = 30
        self.color = color
        self.size = random.randint(2, 4)
    
    def update(self):
        self.x += self.vx
        self.y += self.vy
        self.lifetime -= 1
        self.size = max(1, self.size * 0.95)
    
    def draw(self, surface):
        if self.lifetime > 0:
            alpha = int((self.lifetime / 30) * 255)
            color = (*self.color, alpha)
            s = pygame.Surface((int(self.size * 2), int(self.size * 2)), pygame.SRCALPHA)
            pygame.draw.circle(s, color, (int(self.size), int(self.size)), int(self.size))
            surface.blit(s, (int(self.x - self.size), int(self.y - self.size)))

particles = []

def your_score(score):
    value = score_font.render("Your Score: " + str(score), True, YELLOW)
    dis.blit(value, [0, 0])

def draw_background():
    """Draw tiled background"""
    if use_images:
        tile_size = 40
        for x in range(0, WIDTH, tile_size):
            for y in range(0, HEIGHT, tile_size):
                dis.blit(background_tile, (x, y))
    else:
        dis.fill(BLACK)

def our_snake(snake_block, snake_list, x_change, y_change):
    """Draw snake with head pointing in the right direction"""
    # Draw body segments
    for i, segment in enumerate(snake_list[:-1]):
        if use_images:
            dis.blit(snake_body_img, [segment[0], segment[1]])
        else:
            pygame.draw.rect(dis, GREEN, [segment[0], segment[1], snake_block, snake_block])
    
    # Draw head with correct direction
    if len(snake_list) > 0:
        head = snake_list[-1]
        if use_images:
            if y_change < 0:  # Moving up
                dis.blit(snake_head_up, [head[0], head[1]])
            elif y_change > 0:  # Moving down
                dis.blit(snake_head_down, [head[0], head[1]])
            elif x_change < 0:  # Moving left
                dis.blit(snake_head_left, [head[0], head[1]])
            else:  # Moving right (default)
                dis.blit(snake_head_right, [head[0], head[1]])
        else:
            # Draw a slightly brighter head
            pygame.draw.rect(dis, (0, 255, 100), [head[0], head[1], snake_block, snake_block])

def message(msg, color, y_displace=0):
    mesg = font_style.render(msg, True, color)
    # Center the message
    text_rect = mesg.get_rect(center=(WIDTH/2, HEIGHT/2 + y_displace))
    dis.blit(mesg, text_rect)

def create_food_particles(x, y):
    """Create particle effects when food is eaten"""
    for _ in range(15):
        particles.append(Particle(x + snake_block/2, y + snake_block/2, (255, 200, 0)))

def update_particles():
    """Update and draw all particles"""
    for particle in particles[:]:
        particle.update()
        if particle.lifetime <= 0:
            particles.remove(particle)
        else:
            particle.draw(dis)

def select_speed():
    intro = True
    speed = 15
    while intro:
        dis.fill(BLACK)
        message("Select Difficulty:", WHITE, -50)
        message("1. Easy", GREEN, 0)
        message("2. Medium", YELLOW, 50)
        message("3. Hard", RED, 100)
        pygame.display.update()

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_1:
                    speed = 10
                    intro = False
                elif event.key == pygame.K_2:
                    speed = 15
                    intro = False
                elif event.key == pygame.K_3:
                    speed = 25
                    intro = False
    return speed

def gameLoop(snake_speed):
    global particles
    game_over = False
    game_close = False
    game_over_sound_played = False

    x1 = WIDTH / 2
    y1 = HEIGHT / 2

    x1_change = 0
    y1_change = 0

    snake_List = []
    Length_of_snake = 1
    prev_length = 1

    foodx = round(random.randrange(0, WIDTH - snake_block) / 10.0) * 10.0
    foody = round(random.randrange(0, HEIGHT - snake_block) / 10.0) * 10.0

    while not game_over:

        while game_close == True:
            # Play game over sound only once
            if not game_over_sound_played and game_over_sound:
                game_over_sound.play()
                game_over_sound_played = True
            
            draw_background()
            message("You Lost! Press C-Play Again or Q-Quit", RED)
            your_score(Length_of_snake - 1)
            pygame.display.update()

            for event in pygame.event.get():
                if event.type == pygame.KEYDOWN:
                    if event.key == pygame.K_q:
                        game_over = True
                        game_close = False
                    if event.key == pygame.K_c:
                        particles = []  # Clear particles on restart
                        speed = select_speed()
                        gameLoop(speed)

        x1_change_prev = x1_change
        y1_change_prev = y1_change

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                game_over = True
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_LEFT:
                    if x1_change_prev != snake_block:
                        x1_change = -snake_block
                        y1_change = 0
                elif event.key == pygame.K_RIGHT:
                    if x1_change_prev != -snake_block:
                        x1_change = snake_block
                        y1_change = 0
                elif event.key == pygame.K_UP:
                    if y1_change_prev != snake_block:
                        y1_change = -snake_block
                        x1_change = 0
                elif event.key == pygame.K_DOWN:
                    if y1_change_prev != -snake_block:
                        y1_change = snake_block
                        x1_change = 0

        if x1 >= WIDTH or x1 < 0 or y1 >= HEIGHT or y1 < 0:
            game_close = True
        
        x1 += x1_change
        y1 += y1_change
        
        # Draw background
        draw_background()
        
        # Draw food with image
        if use_images:
            dis.blit(food_img, [foodx, foody])
        else:
            pygame.draw.rect(dis, RED, [foodx, foody, snake_block, snake_block])
        
        snake_Head = []
        snake_Head.append(x1)
        snake_Head.append(y1)
        snake_List.append(snake_Head)
        
        if len(snake_List) > Length_of_snake:
            del snake_List[0]

        for x in snake_List[:-1]:
            if x == snake_Head:
                game_close = True

        our_snake(snake_block, snake_List, x1_change, y1_change)
        
        # Update and draw particles
        update_particles()
        
        your_score(Length_of_snake - 1)

        pygame.display.update()

        if x1 == foodx and y1 == foody:
            # Play eating sound
            if eat_sound:
                eat_sound.play()
            
            # Create particle effect
            create_food_particles(foodx, foody)
            
            foodx = round(random.randrange(0, WIDTH - snake_block) / 10.0) * 10.0
            foody = round(random.randrange(0, HEIGHT - snake_block) / 10.0) * 10.0
            Length_of_snake += 1
            
            # Play level up sound every 5 points
            if (Length_of_snake - 1) % 5 == 0 and level_up_sound and Length_of_snake > 1:
                level_up_sound.play()

        current_speed = snake_speed
        keys = pygame.key.get_pressed()
        if (x1_change < 0 and keys[pygame.K_LEFT]) or \
           (x1_change > 0 and keys[pygame.K_RIGHT]) or \
           (y1_change < 0 and keys[pygame.K_UP]) or \
           (y1_change > 0 and keys[pygame.K_DOWN]):
            current_speed *= 2

        clock.tick(current_speed)

    pygame.quit()
    sys.exit()

speed = select_speed()
gameLoop(speed)
