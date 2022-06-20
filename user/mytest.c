// #include "kernel/types.h"
// #include "user/user.h"
#include "include/stdio.h"
#include "include/stdlib.h"
#include "include/unistd.h"
#include "kernel/types.h"


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

// esc sequence
#define term_cursor_location(x,y) fprintf(stdout, "\033[%d;%dH" ,y,x)

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


void TestColor() {
  /*测试颜色*/
  printf(LIGHT_YELLOW"Yello\n"COLOR_NONE);
  printf(LIGHT_PURPLE"Purple\n"COLOR_NONE);
  printf(BACK_YELLOW"Back_Yello");
  printf(COLOR_NONE"\n");
  fflush(stdout);
}


void TestBuffer() {
  /*测试缓冲区*/
    // for (int n = 1; n <= 1000; n++) {
  //   printf("%d\n", n);
  //   for(int i=1; i <= 100; i++)
  //     printf("-", i);
  // }
  printf("buffer-printf!!!\n");
  char c;
  read(0, &c, 1);
}

void TestMoveCursor() {
  term_cursor_location(40, 40);
  printf("Hello World\n");
  char c;
  printf("starting\n");
  fflush(stdout);
  read(0, &c, 1);
  fprintf(stdout,CLEAR_SCREEN);
  InitializeGame();
  PrintGame(map);
  fflush(stdout);
}

void TestPointerSize() {
    /*测试指针size*/
  printf("sizeofvoid*: %d\n", sizeof(void*));
  printf("sizeofchar*: %d\n", sizeof(char*));
  printf("sizeofuint64: %d\n", sizeof(uint64));
  fflush(stdout);
}


void TestClone() {

}

void thread(void *arg) {
	int id = *(int*)arg;
	printf("thread %d: started...\n", id);
  // for(int i =0 ;;i++) {
  //   printf("t");
  // }
  fflush(stdout);
  exit(0);
  // for(int i=0; ; i++) {
  //       write(1, "t", 1);
  // }
}


int main(){

  printf("starting...\n");
  
  void* stack1;
  stack1 = (void*) malloc(4096);
  int* args1;
  args1 = (int*) malloc(4);
  *args1 = 1;

  // void* stack2;
  // stack2 = (void*) malloc(4096);
  // int* args2;
  // args2 = (int*) malloc(4);
  // *args2 = 2;
  // printf("user> function thread address:%p\n", &thread);
  // printf("user> function thread:%p\n", thread);
  // // printf("user> function TestPointerSize:%p\n", &TestPointerSize);
  // printf("user> stack:%p\n", stack);
  // printf("user> args:%p\n", args);
  clone(&thread, args1, stack1);
  // clone(&thread, args2, stack2);
  // printf("user> after clone\n");
  // fflush(stdout);
  char c;
  read(0, &c, 1);
  printf("user> after clone\n");
  printf("%s\n", c);
  // wait();
  exit(0);
  return 0;

}