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
    music_path = resource_path('background_music.mp3')
    pygame.mixer.music.load(music_path)
    pygame.mixer.music.play(-1)  # -1 means loop indefinitely
except pygame.error as e:
    print(f"Could not load music: {e}")

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

def your_score(score):
    value = score_font.render("Your Score: " + str(score), True, YELLOW)
    dis.blit(value, [0, 0])

def our_snake(snake_block, snake_list):
    for x in snake_list:
        pygame.draw.rect(dis, GREEN, [x[0], x[1], snake_block, snake_block])

def message(msg, color, y_displace=0):
    mesg = font_style.render(msg, True, color)
    # Center the message
    text_rect = mesg.get_rect(center=(WIDTH/2, HEIGHT/2 + y_displace))
    dis.blit(mesg, text_rect)

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
    game_over = False
    game_close = False

    x1 = WIDTH / 2
    y1 = HEIGHT / 2

    x1_change = 0
    y1_change = 0

    snake_List = []
    Length_of_snake = 1

    foodx = round(random.randrange(0, WIDTH - snake_block) / 10.0) * 10.0
    foody = round(random.randrange(0, HEIGHT - snake_block) / 10.0) * 10.0

    while not game_over:

        while game_close == True:
            dis.fill(BLACK)
            message("You Lost! Press C-Play Again or Q-Quit", RED)
            your_score(Length_of_snake - 1)
            pygame.display.update()

            for event in pygame.event.get():
                if event.type == pygame.KEYDOWN:
                    if event.key == pygame.K_q:
                        game_over = True
                        game_close = False
                    if event.key == pygame.K_c:
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
        dis.fill(BLACK)
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

        our_snake(snake_block, snake_List)
        your_score(Length_of_snake - 1)

        pygame.display.update()

        if x1 == foodx and y1 == foody:
            foodx = round(random.randrange(0, WIDTH - snake_block) / 10.0) * 10.0
            foody = round(random.randrange(0, HEIGHT - snake_block) / 10.0) * 10.0
            Length_of_snake += 1

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
