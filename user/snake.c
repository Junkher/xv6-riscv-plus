#include "include/stdio.h"
#include "include/stdlib.h"
#include "include/unistd.h"
#include "kernel/types.h"
#include "include/termios.h"
#include "include/math.h"

#define SIZE 25
#define MAX_X SIZE*2
#define MAX_Y SIZE
#define MIN_X 2
#define MIN_y 2
#define BOARD MAX_X + 4
#define maxLength 100
#define true 1
#define false 0
#define chrBorder "█"
#define chrSnake "●"
#define chrFood "★"
#define chrExit ('x')
#define chrRestart ('r')
#define chrStop ('b')
#define chrContinue ('c')
#define NIGHTMARE 1
#define HARD 2
#define MIDDLE 3
#define EASY 4
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
	Pos pos[maxLength];
	Dir dir;
	Dir nextDir;
	int length;
	int head;
} Snake;


Pos food;
Snake snake;
GameStatus gameStatus;
// if the snake eat the food in this time
bool eaten = 0;
int  difficulty = 5;

// 清屏
#define CLEAR_SCREEN    fprintf(stdout,  "\033[1;1H\033[2J")

// 控制光标位置
#define term_cursor_location(x,y) fprintf(stdout, "\033[%d;%dH" ,y,x)

// 控制IO
struct termios termios, original_termios;


void init();
void init_snake(); 
void draw_border();
void draw_on_pos(Pos pos, char* chr);

void print_score();
void print_help();

Pos getRandomPos();
Pos getFoodPos();
bool isPosEqual(const Pos p1, const Pos p2);
bool inSnake(const Pos pos);


void gameOver();
void gameVictory();

void restart();

void cursor_to_bottom()
{
  term_cursor_location(1, MAX_Y + 1);
}

void cursor_to_center()
{
  term_cursor_location(1 + MAX_X/2, 1 + MAX_Y/2);
}

// 原点是(1,1)
void cursor_to_origin()
{
  term_cursor_location(1, 1);
}

void init_term(){
  tcgetattr(0, &termios);
  original_termios = termios;
  cfmakeraw(&termios);
  tcsetattr(0, TCSANOW, &termios);
  // 隐藏光标
  printf("\033[?25l");
  // printf("?????\n");
  fflush(stdout);
}

void restore_term(){
  tcsetattr(0, TCSANOW, &original_termios);
  // 显示光标
  printf("\033[?25h");
  // printf(CLEAR_SCREEN);
  fflush(stdout);
}

Pos getRandomPos()
{
	Pos pos;
	pos.x = random(2, MAX_X);
	pos.y = random(2, MAX_Y);
	return pos;
}

Pos getFoodPos()
{
	Pos pos = getRandomPos();
	while(inSnake(pos)){
		pos = getRandomPos();
	}
	// // if the pos is not in the center religion
	// // then we get the pos again.
	// if(pos.x < MAX_X / 4 || pos.x > MAX_X * 3 / 4
	// 	|| pos.y < MAX_Y / 4 || pos.y > MAX_Y * 3 / 4)
	// {
	// 	Pos pos = getRandomPos();
	// 	while(inSnake(pos)){
	// 		pos = getRandomPos();
	// 	}	
	// }

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
		int cur = (snake.head + maxLength - i) % maxLength;
		if(isPosEqual(snake.pos[cur], pos))
			return true;
	}
	return false;
}



void init_snake() 
{
  snake.dir = RIGHT;
  snake.nextDir = RIGHT;
  snake.length = 1;
	snake.head = 0;
  snake.pos[0].x = SIZE / 2;
	snake.pos[0].y = SIZE / 2;
  // snake.pos[1].x = SIZE / 2 + 1;
	// snake.pos[1].y = SIZE / 2 + 1;
  // snake.pos[2].x = SIZE / 2 + 2;
	// snake.pos[2].y = SIZE / 2 + 2;
  // for(int i = 0; i < snake.length; i++) {
  //   draw_on_pos(snake.pos[i], chrSnake);
  // }
  // fflush(stdout);
}

// void draw_snake()
// {
//   for(int i = 0; i < snake.length; i++) {
//     draw_on_pos(snake.pos[i], chrSnake);
//   }
// }

// 打印边界
void draw_border()
{
int i, j;
for(i = 1; i <= MAX_X + 1; i++)
{
  for(j = 1; j <= MAX_Y + 1; j++)
  {
    if(i == 1 || j == 1 || i == MAX_X + 1 || j == MAX_Y + 1)
    {
      // MoveToPos(i * 2, j);
      term_cursor_location(i, j);
      printf(chrBorder);
    }
  }
}
//  printf("\n");
}

void draw_face()
{
  Pos p;
  p.x = 3;
  p.y = 3;
  for(int i = 0; i < 10; i++ ) {
    p.x = MAX_X/2 - 5 -i;
    p.y = 10;
    draw_on_pos(p, chrBorder);
  }
  for(int i = 0; i < 10; i++ ) {
    p.x = i + MAX_X/2 + 5;
    p.y = 10;
    draw_on_pos(p, chrBorder);
  }


}

// 初始化
void init() {
  CLEAR_SCREEN;
  // init_map();
  init_snake();
  food = getFoodPos();
  draw_border();
  // draw_face();
  draw_on_pos(food, chrFood);
  print_score();
  print_help();
  gameStatus = RUNNING;
  fflush(stdout);
}

void restart() {
  CLEAR_SCREEN;
  init_snake();
  food = getFoodPos();
  // difficulty = EASY;
  draw_on_pos(food, chrFood);
  draw_border();
  print_score();
  print_help();
  gameStatus = RUNNING;
  fflush(stdout);
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

  // 判断是否撞墙或撞到自己
  if(inSnake(newHeadPos)
    || newHeadPos.x < 2 || newHeadPos.x > MAX_X
    || newHeadPos.y < 2 || newHeadPos.y > MAX_Y
  )
  {
      cursor_to_origin();
      // printf("head y pos:%d", snake.pos[snake.head].y);
      gameStatus = OVER;
      return;
  }
  
  // 移动蛇头
  snake.head = (snake.head + 1) % maxLength ;
  snake.pos[snake.head] = newHeadPos;

  // 判断是否吃到食物
  if(isPosEqual(newHeadPos, food))
	{
		eaten = 1;
		snake.length ++;
		food = getFoodPos();
		if(snake.length > maxLength)
		{
			gameStatus = VICTORY;
		}
	}
	else
		eaten = 0;

}


void draw_on_pos(Pos pos, char* chr)
{
  cursor_to_origin();
  term_cursor_location(pos.x, pos.y);
  printf(chr);
}

void draw()
{
  cursor_to_origin();
	if(gameStatus != RUNNING)
		return;
  // 画出蛇头
	draw_on_pos(snake.pos[snake.head], chrSnake);
	if(eaten == 0){
    // 擦除蛇尾
    int tail = (snake.head + maxLength - snake.length) % maxLength;
		draw_on_pos(snake.pos[tail], " ");
	} else {
    // 新的食物
    draw_on_pos(food, chrFood);
    print_score();
  }
  
}

void game()
{
  while (gameStatus == WAITING)
  {
    sleep(5);
  }

  if(gameStatus == OVER)
		gameOver();
	else if(gameStatus == VICTORY)
		gameVictory();
	else if(gameStatus == RUNNING)
	{
		logic();
		draw();
	}
	// else if(gameStatus == WAITING)
	// {
  //   // fflush(stdout);
	// 	// while (gameStatus == WAITING)
  //   // {}
	// }
  fflush(stdout);
}

//multi-thread
void logic_thread(void *arg)
{
	// int id = *(int*)arg;
	// printf("thread %d: started...\n", id);
  while(sleep(difficulty), gameStatus != EXIT){
    game();
    // draw();
    // fflush(stdout);
  }
	exit(0);
}


void gameOver()
{
	// cursor_to_bottom();
  // CLEAR_SCREEN;
  term_cursor_location(BOARD, 8);
  printf("寄...");
  // term_cursor_location(0, 0);
  term_cursor_location(BOARD , 9);
  printf("按 \'%c\' 重新开始 或者  \'%c\' 退出\n", chrRestart, chrExit);
	gameStatus = WAITING;
}

void gameVictory()
{
	// CLEAR_SCREEN;
  term_cursor_location(BOARD, 8);
  printf("芜湖!!!!!!!!\n");
  term_cursor_location(BOARD, 9);
	// printf("Press \'%c\' to restart OR press \'%c\' to exit\n", chrRestart, chrExit);
  printf("按 \'%c\' 重新开始 或者  \'%c\' 退出\n", chrRestart, chrExit);
	gameStatus = WAITING;
}

void print_score()
{
  term_cursor_location(MAX_X + 4, 1);
  printf("当前分数: %d", snake.length * 10 -10);
}

void print_help()
{
  term_cursor_location(MAX_X + 4, 2);
  printf("-------------------------------------\n");
  term_cursor_location(MAX_X + 4, 4);
  printf("w, a, s, d 控制移动");
  term_cursor_location(MAX_X + 4, 5);
  printf("1, 2, 3, 4 调节速度");
  term_cursor_location(MAX_X + 4, 6);
  printf("b 暂停游戏    c 继续游戏");
  term_cursor_location(MAX_X + 4, 7);
  printf("x 退出游戏");

}


void end(){
  CLEAR_SCREEN;
  cursor_to_origin();
  printf("Bye\n");
  restore_term();
  exit(0);
}

void main_thread()
{
	while(gameStatus != EXIT)
	{
		char c = ' ';
		read(0, &c, 1);
		if(gameStatus == WAITING)
		{
			while(c != chrRestart && c != chrExit && c != chrContinue)
			{
				read(0, &c, 1);
			}
			if(c == chrRestart)
			{
				// init();
        restart();
				continue;
			}
			else if(c == chrExit)
			{
				gameStatus = EXIT;
				break;
			}
      else
      {
        gameStatus = RUNNING;
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
        case '1':
          difficulty = EASY;
          break;
        case '2':
          difficulty = MIDDLE;
          break;
        case '3':
          difficulty = HARD;
          break;
        case '4':
          difficulty = NIGHTMARE;
          break;
        case chrExit:
					gameStatus = EXIT;
					break;
        case chrStop:
          gameStatus = WAITING;
          break;
				default:
					continue;
			}
		}
	}
}


int main(int argc, char **argv)
{

	// void *stacks[2];
	// int *args[2];

	// for (int i=0; i<1; i++) {
	// 	stacks[i] = (void*) malloc(4096);
	// 	args[i] = (int*) malloc(4);
	// 	*args[i] = i;
	// }
 
  //为子线程的函数运行分配栈空间  
  void* stack;
  stack = (void*) malloc(4096);
  int* arg;
  arg = (int*) malloc(4);
  *arg = 1;

  // ioctl
  init_term();

  // 游戏初始化
  init();

  // 运行子线程，根据状态，进行绘制时的逻辑判断
	clone(logic_thread, arg, stack);

  // 运行主线程，用于接收键盘输入，修改当前状态 
  main_thread();

  // 等待子线程结束
  join();

  // 退出
	end();
  return 0;
}
