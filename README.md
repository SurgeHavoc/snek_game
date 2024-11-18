# snek_game

## Description

A game with origins dating back to the 70's "Snake" is a classic arcade game that's both simple and fun. And as such, snek_game is a modern take on this type of game and made in C++ using teh SDL2 library.
In snek_game, the player can control a "snake" that can move around in a box-like level while trying to gather the "apples" that spawn randomly on the screen.
The objective is to navigate the "snake" without colliding with any obstacles, walls, or the "snake" itself.
As the player progresses, the score will increase, and so will the speed of the game. This ramps up the difficulty, providing an escalating challenge.

### Motivation

Retro arcade games are some of my favorite games ever. Snake was an easy pick to make because it is simple, fun, and iconic. I also developed this project to enhance my understanding of using C++ with SDL2 and game architecture.
This project also happened to provide me with an opportunity to implement a cap on frame rate, leading me to eventually get into learning about variable frame rate.

### What I Learned
- Frame rate capping
- Implementing collision detection
- Rendering graphics in SDL2

## Usage
### Starting the Game
- Run the .exe using ./snek_game

### Controls
- Arrow keys control the snake.
- Press the escape key to exit the game.

### Gameplay
Navigate the snake and collect the apple (the red squares) that appears randomly on the screen.
Each apple increases the snake length by one unit.
The speed increases as the score increases.
Avoid colliding with obstacles, the walls, or the snake itself to prevent a game over.
Current score can be tracked in the window's title bar on the top left of it.

![image](https://github.com/user-attachments/assets/070f7dc1-991a-4deb-8862-37dd65410a23)


## Credits
- I worked on this project solo.

## Acknowledgments
SDL2 Library - [SDL website](https://www.libsdl.org/)
- I followed a C tutorial that I found a while ago.
