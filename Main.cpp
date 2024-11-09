#include <SDL.h>
#include <iostream>

constexpr int SCREEN_WIDTH = 680;
constexpr int SCREEN_HEIGHT = 400;

constexpr int WALL_THICKNESS = 20;
constexpr int CELL_WIDTH = 20;
constexpr int CELL_HEIGHT = 20;
constexpr int CELL_COUNT = ((SCREEN_WIDTH - WALL_THICKNESS * 2) * (SCREEN_HEIGHT - WALL_THICKNESS * 2)) / (CELL_WIDTH * CELL_HEIGHT);

constexpr int SNAKE_START_X = 200;
constexpr int SNAKE_START_Y = 200;

class Game
{
public:
	Game();
	~Game();
	void run();
private:
	void initialize();
	void terminate(int exit_code);
	void handle_input();
	void draw_walls();
	void draw_snake();
	void spawn_snake();
	void move_snake();
	void change_direction(SDL_Keycode new_direction);
	void handle_collisions();
	void spawn_food();
	void draw_food();
	void display_score();

	SDL_Renderer* renderer = {};
	SDL_Window* window = {};
	bool running;
	SDL_Rect snake[CELL_COUNT] = {};
	int dx;
	int dy;
	bool game_over;
	SDL_Rect food;
	int score;
	bool direction_changed;
};

Game::Game() : running(true), dx(CELL_WIDTH), dy(0), game_over(false), score(0)
{
	food.w = CELL_WIDTH;
	food.h = CELL_HEIGHT;
}

Game::~Game()
{
	terminate(EXIT_SUCCESS);
}

void Game::initialize()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "Error: Failed to initialize SDL: " << SDL_GetError() << std::endl;
		terminate(EXIT_FAILURE);
	}

	// Create the game window.
	window = SDL_CreateWindow("Score: 0", 
				SDL_WINDOWPOS_UNDEFINED, 
				SDL_WINDOWPOS_UNDEFINED, 
				SCREEN_WIDTH, 
				SCREEN_HEIGHT, 
				SDL_WINDOW_SHOWN);
	if (!window)
	{
		std::cout << "Error: Failed to open " << SCREEN_WIDTH << " x " << SCREEN_HEIGHT << " window: " << SDL_GetError() << std::endl;
		terminate(EXIT_FAILURE);
	}

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (!renderer)
	{
		std::cout << "Error: Failed to create renderer: " << SDL_GetError() << std::endl;
		terminate(EXIT_FAILURE);
	}
}

void Game::terminate(int exit_code)
{
	if (renderer)
	{
		SDL_DestroyRenderer(renderer);
	}
	if (window)
	{
		SDL_DestroyWindow(window);
	}
	SDL_Quit();
	exit(exit_code);
}

void Game::handle_input()
{
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		// Checks for input to exit the game.
		if (e.type == SDL_QUIT || (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE))
		{
			running = false;
		}

		// Checks for input to move the snake in a direction based on the key pressed.
		if(e.type == SDL_KEYDOWN && !direction_changed && (e.key.keysym.sym == SDLK_UP
														|| e.key.keysym.sym == SDLK_DOWN
														|| e.key.keysym.sym == SDLK_LEFT
														|| e.key.keysym.sym == SDLK_RIGHT))
		{
			change_direction(e.key.keysym.sym);
			direction_changed = true;
		}
	}
}

void Game::draw_walls()
{
	// Color of the walls are set to a light blue.
	SDL_SetRenderDrawColor(renderer, 10, 209, 205, 255);

	// On game over, change the color of the walls to red.
	if (game_over)
	{
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	}

	// Create left wall.
	SDL_Rect block = {0, 0, WALL_THICKNESS, SCREEN_HEIGHT};
	SDL_RenderFillRect(renderer, &block);

	// Create right wall.
	block.x = SCREEN_WIDTH - WALL_THICKNESS;
	SDL_RenderFillRect(renderer, &block);

	// Create top wall.
	block = {0, 0, SCREEN_WIDTH, WALL_THICKNESS};
	SDL_RenderFillRect(renderer, &block);

	// Create bottom wall.
	block.y = SCREEN_HEIGHT - WALL_THICKNESS;
	SDL_RenderFillRect(renderer, &block);
}

void Game::draw_snake()
{
	// Create the snake.
	for (int i = 0; i < sizeof(snake) / sizeof(snake[0]); i++)
	{
		// NULL check.
		if (snake[i].w == 0)
		{
			break;
		}

		// On game over, change the color of the snake to red.
		if (game_over)
		{
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		}
		else
		{
			SDL_SetRenderDrawColor(renderer, 0, 128, 0, 255);
		}
		SDL_RenderFillRect(renderer, &snake[i]);

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderDrawRect(renderer, &snake[i]);
	}
}

void Game::spawn_snake()
{
	// Initialize parts of snake to 0.
	for (int i = 0; i < sizeof(snake) / sizeof(snake[0]); i++)
	{
		snake[i] = {0, 0, 0, 0};
	}

	// Set element 0 to head.
	snake[0] = {SNAKE_START_X, SNAKE_START_Y, CELL_WIDTH, CELL_HEIGHT};

	// Create the snake body.
	for (int i = 1; i < 5; i++)
	{
		snake[i] = snake[0];
		snake[i].x = snake[0].x - (CELL_WIDTH * i);
	}
}

void Game::move_snake()
{
	// Check if game is over.
	if (game_over)
		return;
	// Shift elements to the right to make room for the snake head.
	for (int i = sizeof(snake) / sizeof(snake[0]) - 1; i > 0; i--)
	{
		snake[i] = snake[i - 1];
	}

	// Insert the snake head's new position at the beginning of the snake array.
	snake[0].x = snake[1].x + dx;
	snake[0].y = snake[1].y + dy;
	snake[0].w = CELL_WIDTH;
	snake[0].h = CELL_HEIGHT;

	/* If the snake touches the food, make the snake grow by not removing the tail
	and spawn the food in a new random location.
	*/
	if (snake[0].x == food.x && snake[0].y == food.y)
	{
		spawn_food();
		score++;
		display_score();
	}
	else
	{
		// Remove the tail of the snake and initialize element in front of it.
		for (int i = 5; i < sizeof(snake) / sizeof(snake[0]); i++)
		{
			if (snake[i].w == 0)
			{
				snake[i - 1] = {0, 0, 0, 0};
				break;
			}
		}
	}
	handle_collisions();
	direction_changed = false;
}

void Game::change_direction(SDL_Keycode new_direction)
{
	// Initialize snake directions.
	int going_up = dy == -CELL_HEIGHT;
	int going_down = dy == CELL_HEIGHT;
	int going_left = dx == -CELL_WIDTH;
	int going_right = dx == CELL_WIDTH;

	// Checks for directional input to change the direction of the snake.
	if (new_direction == SDLK_UP && !going_down)
	{
		dx = 0;
		dy = -CELL_HEIGHT;
	}

	if (new_direction == SDLK_DOWN && !going_up)
	{
		dx = 0;
		dy = CELL_HEIGHT;
	}

	if (new_direction == SDLK_LEFT && !going_right)
	{
		dx = -CELL_WIDTH;
		dy = 0;
	}

	if (new_direction == SDLK_RIGHT && !going_left)
	{
		dx = CELL_WIDTH;
		dy = 0;
	}
}

void Game::handle_collisions()
{
	// Check if snake has collided with itself.
	for (int i = 1; i < sizeof(snake) / sizeof(snake[0]); i++)
	{
		// Exit loop after traversing snake body.
		if (snake[i].w == 0)
			break;
		if (snake[0].x == snake[i].x && snake[0].y == snake[i].y)
		{
			game_over = true;
			return;
		}
	}

	// Check if the snake has collided with the left wall.
	if (snake[0].x < WALL_THICKNESS)
	{
		game_over = true;
		return;
	}

	// Check if the snake has collided with the right wall.
	if (snake[0].x > SCREEN_WIDTH - WALL_THICKNESS - CELL_WIDTH)
	{
		game_over = true;
		return;
	}

	// Check if the snake has collided with the top wall.
	if (snake[0].y < WALL_THICKNESS)
	{
		game_over = true;
		return;
	}

	// Check if the snake has collided with the bottom wall.
	if (snake[0].y > SCREEN_HEIGHT - WALL_THICKNESS - CELL_HEIGHT)
	{
		game_over = true;
		return;
	}
}

void Game::spawn_food()
{
	/* Generate a random number in multiples of 10
	along the x-axis that fits between tht left and right walls.
	*/
	food.x = (rand() % ((SCREEN_WIDTH - CELL_WIDTH - WALL_THICKNESS) / CELL_WIDTH + 1)) * CELL_WIDTH;
	/* Generate a random number in multiples of 10
	along the y-axis that fits between the top and bottom walls.
	*/
	food.y = (rand() % ((SCREEN_HEIGHT - CELL_HEIGHT - WALL_THICKNESS) / CELL_HEIGHT + 1)) * CELL_HEIGHT;

	/* If the random number generated is less than the thickness of the left wall,
	have the food spawn next to the left wall.
	*/
	if (food.x < WALL_THICKNESS)
		food.x = WALL_THICKNESS;
	/* If the random number generated is less than the thickness of the top wall,
	have the food spawn next to the top wall.
	*/
	if (food.y < WALL_THICKNESS)
		food.y = WALL_THICKNESS;

	// Spawn food if not in snake.
	for (int i = 0; i < sizeof(snake) / sizeof(snake[0]); i++)
	{
		if (snake[i].w == 0)
			break;
		if (snake[i].x == food.x && snake[i].y == food.y)
		{
			spawn_food();
			break;
		}
	}
}

void Game::draw_food()
{
	// Make red food.
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderFillRect(renderer, &food);
}

void Game::display_score()
{
	char buffer[20];
	snprintf(buffer, 20, "Score: %d", score);
	SDL_SetWindowTitle(window, buffer);
}

void Game::run()
{
	// Initialize SDL.
	initialize();

	// Initialize snek.
	spawn_snake();

	// Create food.
	spawn_food();

	// Enter the game loop.
	while (running)
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		handle_input();
		move_snake();

		draw_food();
		draw_snake();
		draw_walls();

		SDL_RenderPresent(renderer);

		// Delay before the next iteration.
		if (score >= 3 && score < 6)
		{
			SDL_Delay(140);
		}
		else if (score >= 6 && score < 10)
		{
			SDL_Delay(120);
		}
		else if (score >= 10 && score < 14)
		{
			SDL_Delay(110);
		}
		else if (score >= 14 && score < 18)
		{
			SDL_Delay(100);
		}
		else if (score >= 18 && score < 22)
		{
			SDL_Delay(80);
		}
		else if (score >= 22 && score < 32)
		{
			SDL_Delay(60);
		}
		else if (score >= 32)
		{
			SDL_Delay(50);
		}
		else
		{
			SDL_Delay(150);
		}
	}
	// Make sure the program cleans up on exit.
	terminate(EXIT_SUCCESS);
}

int main(int argc, char* argv[])
{
	Game game;
	game.run();
	return 0;
}