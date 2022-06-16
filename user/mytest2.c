#include "kernel/types.h"
#include "user/user.h"


#define COLOR_NONE         			"\033[m"  
#define RED          				"\033[0;31m"
#define LIGHT_RED    				"\033[1;31m"  //红色高亮
#define LIGHT_RED_INV    			"\033[5;7;31m"  //红色高亮，并反白显示，字体闪烁
#define GREEN        				"\033[0;32m"
#define LIGHT_GREEN  				"\033[1;32m"
#define BLUE         				"\033[0;34m"
#define LIGHT_BLUE   				"\033[1;34m" //蓝色高亮
#define DARY_GRAY    				"\033[1;30m"
#define CYAN         				"\033[0;36m"
#define LIGHT_CYAN   				"\033[1;36m"
#define PURPLE       				"\033[0;35m"
#define LIGHT_PURPLE 				"\033[1;35m"
#define YELLOW        				"\033[0;33m"
#define LIGHT_YELLOW       			"\033[1;33m" //黄色高亮
#define WHITE                                   "\033[0;37m"
#define LIGHT_WHITE        			"\033[1;37m" //白色高亮
#define BACK_YELLOW       "\033[1;43m" //背景黄色
#define CLEAR_SCREEN    "\033[2J"

#define term_cursor_location(x,y) fprintf(1, "\033[%d;%dH" ,y,x)

#define chrSnake ((char)(219))

#define SIZE 40

int map[SIZE][SIZE] = {0};


void InitializeGame()
 {
 	int i, j;
 	for(i = 0; i < SIZE; i++)
 	{
 		if(i == 0||i == SIZE - 1)
 		{
 			for(j = 0; j < SIZE;j++)
			{
				map[i][j] = 1;	 	
			}	
		}
		else
		{
			map[i][0] = 1;
			map[i][SIZE - 1] = 1;
		} 		
	}
 }

void PrintGame(int map[SIZE][SIZE])
{
int i, j;
for(i = 0;i < SIZE;i++)
{
  for(j = 0;j < SIZE;j++)
  {
    if(map[i][j] == 1)
    {
      // MoveToPos(i * 2, j);
      term_cursor_location(i * 2, j);
      printf("██");
    }
  }
}
 printf("\n");
}


int main(){
  // 无缓冲区，每一次printf都需要进入内核write
  printf("xv-riscv-printf!!!\n");
  printf(LIGHT_YELLOW"Yello\n"COLOR_NONE);
  printf(LIGHT_PURPLE"Purple\n"COLOR_NONE);
  printf(BACK_YELLOW"Back_Yello");
  printf(COLOR_NONE"\n");
  // char c;
  // read(0, &c, 1);
  printf(CLEAR_SCREEN);
  InitializeGame();
  PrintGame(map);
  exit(0);
  return 0;

}