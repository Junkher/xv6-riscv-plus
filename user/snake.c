#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

#define MAX_X 40
#define MAX_Y 24
#define MaxSize (1000 + 10)
#define true 1
#define false 0
#define chrSnake ((char)(219))
#define chrFood ((char)(176))
#define chrExit ('x')
#define chrRestart ('y')
typedef enum{DOWN, UP, LEFT, RIGHT} Dir;
typedef enum{BEGINNING, RUNNING, OVER, VICTORY, WAITING, EXIT} GameStatus;
typedef int bool;
typedef struct
{
	int x;
	int y;
} Pos;

typedef struct
{
	Pos pos[MaxSize];
	Dir dir;
	Dir nextDir;
	int length;
	int head;
} Snake;

Pos food;
Snake snake;
// if the snake eat the food in this time
bool eaten = 0;
GameStatus gameStatus;

void main_thread();
Pos getRandomPos();
Pos getFoodPos();
bool isPosEqual(const Pos p1, const Pos p2);
bool inSnake(const Pos pos);
void init();
int game();
void gameOver();
void gameVictory();

// move the snake and eat the food.
void logic();

// draw the snake and food
void draw();
void draw_food();
void draw_snake_head();
void draw_length();
void draw_black_on_tail();

// Change a char to lowercase if c is uppercase
// Nothing will be done if c is lowercase or not a English letter.
char charLower(const char c);


void main_thread()
{
	init();
	set_console_parameters(CONS_NO_BUFFER | CONS_CDEFAULT);
	while(gameStatus != EXIT)
	{
		char c = ' ';
		read(0, &c, 1);
		c = charLower(c);
		if(gameStatus == WAITING)
		{
			while(c != chrRestart && c != chrExit)
			{
				read(0, &c, 1);
				c = charLower(c);
			}
			if(c == chrRestart)
			{
				init();
				continue;
			}
			else
			{
				gameStatus = EXIT;
				break;
			}
			
		}
		else if(gameStatus == RUNNING || gameStatus == BEGINNING)
		{
			switch(c)
			{
				case 'w':
					if(snake.dir != DOWN || snake.length == 1)
						snake.nextDir = UP;
					if(gameStatus == BEGINNING)
						gameStatus = RUNNING;
					break;
				case 'a':
					if(snake.dir != RIGHT || snake.length == 1)				
						snake.nextDir = LEFT;
					if(gameStatus == BEGINNING)
						gameStatus = RUNNING;
					break;
				case 'd':
					if(snake.dir != LEFT || snake.length == 1)				
						snake.nextDir = RIGHT;
					if(gameStatus == BEGINNING)
						gameStatus = RUNNING;
					break;
				case 's':
					if(snake.dir != UP || snake.length == 1)				
						snake.nextDir = DOWN;
					if(gameStatus == BEGINNING)
						gameStatus = RUNNING;
					break;
				case chrExit:
					gameStatus = EXIT;
					break;
				default:
					continue;
			}
		}
	}
}

Pos getRandomPos()
{
	Pos pos;
	pos.x = random(0, MAX_X);
	pos.y = random(0, MAX_Y);
	return pos;
}

Pos getFoodPos()
{
	Pos pos = getRandomPos();
	while(inSnake(pos)){
		pos = getRandomPos();
	}
	// if the pos is not in the center religion
	// then we get the pos again.
	if(pos.x < MAX_X / 4 || pos.x > MAX_X * 3 / 4
		|| pos.y < MAX_Y / 4 || pos.y > MAX_Y * 3 / 4)
	{
		Pos pos = getRandomPos();
		while(inSnake(pos)){
			pos = getRandomPos();
		}	
	}

	return pos;
}

bool isPosEqual(const Pos p1, const Pos p2)
{
	if(p1.x == p2.x && p1.y == p2.y)
		return true;
	return false;
}

bool inSnake(const Pos pos)
{
	int i;
	for(i = 0; i < snake.length - 1; i ++)
	{
		int cur = (snake.head + MaxSize - i) % MaxSize;
		if(isPosEqual(snake.pos[cur], pos))
			return true;
	}
	return false;
}

void init()
{
	clear_screen();
	snake.dir = UP;
	snake.nextDir = UP;
	snake.length = 1;
	snake.head = 0;
	snake.pos[0].x = MAX_X / 2;
	snake.pos[0].y = MAX_Y / 2;
	food = getFoodPos();
	gameStatus = BEGINNING;

	draw_food();
	draw_snake_head();
	draw_length();
}

int game()
{
	if(gameStatus == OVER)
		gameOver();
	else if(gameStatus == VICTORY)
		gameVictory();
	else if(gameStatus == RUNNING)
	{
		logic();
		draw();
	}
	else if(gameStatus == EXIT)
	{
		clear_screen();
		set_console_parameters(CONS_BUFFER | CONS_CDEFAULT);
		exit(0);
	}
	else if(gameStatus == WAITING)
	{
		// do nothing.
	}

	return 0;
}

void gameOver()
{
	clear_screen();
	printf(1, "Game over!\nSnake length: %d\npress %c to restart\npress %c to exit\n",snake.length, chrRestart, chrExit);
	gameStatus = WAITING;
}

void gameVictory()
{
	clear_screen();
	printf(1, "Vicotry!\npress %c to restart\npress %c to exit\n", chrRestart, chrExit);
	gameStatus = WAITING;
}

void logic()
{
	Pos newHeadPos = snake.pos[snake.head];
	snake.dir = snake.nextDir;
	switch(snake.dir)
	{
		case DOWN:
			newHeadPos.y ++;
			break;
		case UP:
			newHeadPos.y --;
			break;
		case RIGHT:
			newHeadPos.x ++;
			break;
		case LEFT:
			newHeadPos.x --;
			break;
		default:
			break;
	}

	// Game over?
	if(inSnake(newHeadPos)
		|| newHeadPos.x < 0 || newHeadPos.x >= MAX_X
		|| newHeadPos.y < 0 || newHeadPos.y >= MAX_Y
		)
	{
		gameStatus = OVER;
	}
	// move the snake to the new position.
	snake.head = (snake.head + 1) % MaxSize;
	snake.pos[snake.head] = newHeadPos;
	if(isPosEqual(newHeadPos, food))
	{
		eaten = 1;
		snake.length ++;
		food = getFoodPos();
		if(snake.length > MaxSize)
		{
			gameStatus = VICTORY;
		}
	}
	else
		eaten = 0;
}

void draw()
{
	if(gameStatus != RUNNING)
		return;
	draw_snake_head();
	if(eaten == 0){
		draw_black_on_tail();
	}
	else
	{
		draw_food();
		draw_length();
	}
}

void draw_food()
{
	set_console_parameters(CONS_NO_BUFFER | CONS_CGREEN);
	write_at(2 * food.x, food.y, chrFood);
	write_at(2 * food.x + 1, food.y, chrFood);
	set_console_parameters(CONS_NO_BUFFER | CONS_CDEFAULT);
}

void draw_snake_head()
{
	set_console_parameters(CONS_NO_BUFFER | CONS_CBLUE);
	int x = snake.pos[snake.head].x;
	int y = snake.pos[snake.head].y;
	write_at(2 * x, y, chrSnake);
	write_at(2 * x + 1, y, chrSnake);
	set_console_parameters(CONS_NO_BUFFER | CONS_CDEFAULT);
}

void draw_length()
{
	set_console_parameters(CONS_NO_BUFFER | CONS_CRED);
	set_cursor(35, MAX_Y);
	printf(1, "Snake length: %d", snake.length);
	set_console_parameters(CONS_NO_BUFFER | CONS_CDEFAULT);
}

void draw_black_on_tail()
{
	int cur = (snake.head + MaxSize - snake.length) % MaxSize;
	int x = snake.pos[cur].x;
	int y = snake.pos[cur].y;
	write_at(2 * x, y, ' ');
	write_at(2 * x + 1, y, ' ');
}

char charLower(char c)
{
	if(c >= 'A' && c <= 'Z')
		return(c - 'A' + 'a');
	return c;
}


//multi-thread


void *thread(void *arg)
{
	sleep(10);
	int id = *(int*)arg;
	printf(1, "thread %d: started...\n", id);

	if(id==0)
	{
		while(sleep(10),gameStatus!=EXIT)
			game();
	}
	else{
		main_thread();
		clear_screen();
		set_console_parameters(CONS_BUFFER | CONS_CDEFAULT);
	}

	exit(0);
}

int main(int argc, char **argv)
{
	int i, j;

	void *stacks[2];
	int *args[2];

	for (i=0; i<2; i++) {
		stacks[i] = (void*) malloc(4096);
		args[i] = (int*) malloc(4);
		*args[i] = i;
	}


	for (i=0; i<2; i++) {
		clone(thread, args[i], stacks[i]);
	}

	for (i=0; i<2; i++) {
		void *_;
		join(&_);
	}

	exit(0);
}
