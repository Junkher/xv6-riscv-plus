#include "include/unistd.h"
#include "include/stdio.h"
#include "include/stdlib.h"
#include "include/termios.h"

#define true  1
#define false 0
typedef int bool;

struct termios termios, original_termios;

/*设置输出背景色,1表示粗体高亮 */ 
#define BACK_NONE       "\033[0m"
#define BACK_BLA        "\033[1;40m" //黑色
#define BACK_RED        "\033[1;41m" //红色
#define BACK_GREEN      "\033[1;42m" //绿色
#define BACK_YELLOW     "\033[1;43m" //黄色
#define BACK_BLUE       "\033[1;44m" //蓝色
#define BACK_PUR        "\033[1;45m" //紫色
#define BACK_CYA        "\033[1;46m" //青色
#define BACK_WHI        "\033[1;47m" //白色

#define CLEAR_SCREEN  "\033[H\033[J"

#define UP    0
#define DOWN  1
#define RIGHT 2
#define LEFT  3
int dx[4] = {  0, 0, 1, -1};
int dy[4] = { -1, 1, 0,  0};

int board[4][4];
int score;
  

int rand(int n) {
  static int lcg = 0;
  int x;
  lcg = 1664525 * lcg + 1013904223;
  x = lcg % n;
  return x < 0 ? x + n : x;
}

void print_int(int d)
{
  printf("%4d", d);
}

void changeColor(int b){
  if(b==0){
    // 清除颜色
    printf(BACK_NONE);
  } else {
    switch((b-1)%6){
    case 0:
      // printf("\033[1;43m");
      printf(BACK_YELLOW);
      break;
    case 1:
      printf("\033[1;42m");
      break;
    case 2:
      printf("\033[1;46m");
      break;
    case 3:
      printf("\033[1;44m");
      break;
    case 4:
      printf("\033[1;45m");
      break;
    case 5:
      printf("\033[1;41m");
      break;
    }
  }
}

void resetColor(){
  changeColor(0);
}

void showBoard(){
  int i,j,b;
  printf("showBoard\n");
  // clear screen
  // printf("\033[2J");
  printf(CLEAR_SCREEN);
  // fflush(stdout);
  // printf("\033[1;1H");
  // draw
  for(i=0; i<4; i++){
    printf("+------+------+------+------+\n");
    for(j=0; j<4; j++){
      printf("|");
      changeColor(board[i][j]);
      printf("      ");
      resetColor();
    } printf("|\n");
    for(j=0; j<4; j++){
      printf("|");
      b = board[i][j];
      changeColor(b);
      if(b != 0)
        printf(" %4d ", 1 << b);
      else
        printf("      ");
      resetColor();
    } printf("|\n");
    for(j=0; j<4; j++){
      printf("|");
      changeColor(board[i][j]);
      printf("      ");
      resetColor();
    } printf("|\n");
  }

  printf("+------+------+------+------+\n");
  printf("showBoard finished\n");
}


bool isValid(int x, int y){
  if(x < 0 || 4 <= x)
    return false;
  if(y < 0 || 4 <= y)
    return false;
  return true;
}

bool movable(int dir){
  int i,j;
  int ay,ax;
  for(i=0; i<4; i++){
    for(j=0; j<4; j++){
      ay = i+dy[dir]; ax = j+dx[dir];
      if(isValid(ax, ay) && board[i][j]!=0){
        if(board[ay][ax]==0 || board[ay][ax]==board[i][j]){
          return true;
        }
      }
    }
  }
  return false;
}

bool gameover(){
  int i,j;
  for(i=0; i<4; i++){
    for(j=0; j<4; j++){
      if(board[i][j] == 11)
        return true;
    }
  }
  for(i=0; i<4; i++){
    if(movable(i))
      return false;
  }
  return true;
}

void putNum(){
  int n, b;
  if(rand(10)==0)
    n=2;
  else
    n=1;

  do{
    b = rand(16);
  } while(board[b%4][b/4] != 0);
  board[b%4][b/4] = n;
}

void init_term(){
  printf("In init_term, termios_p: %d\n", &termios);
  printf("In init_term, before tcgetattr, termios: %d\n", termios.c_lflag);
  tcgetattr(0, &termios);
  printf("In init_term, after tcgetattr, termios: %d\n", termios.c_lflag);
  original_termios = termios;
  cfmakeraw(&termios);
  tcsetattr(0, TCSANOW, &termios);
  printf("tcsetattr finished\n");
  // 隐藏光标
  printf("\033[?25l");
  // printf("?????\n");
  fflush(stdout);
  printf("init_term finished\n");
}

void restore_term(){
  tcsetattr(0, TCSANOW, &original_termios);
  // 显示光标
  printf("\033[?25h");
  printf(CLEAR_SCREEN);
  fflush(stdout);
}


void init(){
  int i,j,b;
  for(i=0; i<4; i++)
    for(j=0; j<4; j++)
      board[i][j] = 0;

  score = 0;

  b = rand(16);
  board[b%4][b/4] = 1;
  
  putNum();
  // printf("init...\n");
  // clear screen
  // printf("\033[2J");
  // printf("init...\n");
}

void end(){
  restore_term();
  exit(0);
}


int c2dir(char c){
  switch(c){
  case 'w': case 'W': 
    return 0;
  case 's': case 'S': 
    return 1;
  case 'd': case 'D': 
    return 2;
  case 'a': case 'A': 
    return 3;
  default:
    return -1;
  }
}

void push(int d){
  bool f;
  int i,j,ay,ax;
  do{
    f=false;
    for(i=0; i<4; i++){
      for(j=0; j<4; j++){
        ay=i+dy[d]; ax=j+dx[d];
        if(isValid(ax,ay) && board[ay][ax]==0 && board[i][j]!=0){
          board[ay][ax] = board[i][j];
          board[i][j] = 0;
          f=true;
        }
      }
    }
  } while(f);
}

void merge(int d){
  int i,j;
  switch(d){
  case UP:
    for(j=0;j<4;j++){
      for(i=0;i<3;){
        if(board[i][j]==board[i+1][j] && board[i][j]!=0){
          board[i][j]++;
          score += 1 << board[i][j];
          board[i+1][j]=0;
          i+=2;
        } else {
          i++;
        }
      }
    }
    break;
  case DOWN:
    for(j=0;j<4;j++){
      for(i=3;i>0;){
        if(board[i][j]==board[i-1][j] && board[i][j]!=0){
          board[i][j]++;
          score += 1 << board[i][j];
          board[i-1][j]=0;
          i-=2;
        } else {
          i--;
        }
      }
    }
    break;
  case RIGHT:
    for(i=0;i<4;i++){
      for(j=3;j>0;){
        if(board[i][j]==board[i][j-1] && board[i][j]!=0){
          board[i][j]++;
          score += 1 << board[i][j];
          board[i][j-1]=0;
          j-=2;
        } else {
          j--;
        }
      }
    }
    break;
  case LEFT:
    for(i=0;i<4;i++){
      for(j=0;j<3;){
        if(board[i][j]==board[i][j+1] && board[i][j]!=0){
          board[i][j]++;
          score += 1 << board[i][j];
          board[i][j+1]=0;
          j+=2;
        } else {
          j++;
        }
      }
    }
    break;
  }
}

void Move(int d){
  push(d);
  merge(d);
  push(d);
}

int main(){
  int d;
  init_term();
  init();
  printf("game start!!!\n");
  do{
    for(int i = 0; i < 4; i++) {
      for(int j = 0; j <4; j++) {
          printf("%d ", board[i][j]);
      }
      printf("\n");
    }
    showBoard();
    printf("score: %d\n", score);
    printf("(enter \'q\' to exit...)\n");
    // printf("> ");
    // fflush(stdout);
    do{
      char c;
      // c = getchar();
      printf("waiting input...\n");
      fflush(stdout);
      read(0, &c, 1);
      printf("your input is%d\n",c);
      if (c == 'q')
        // exit(0);
        end();
      d = c2dir(c);
    } while(d==-1 || !movable(d));
    Move(d);
    putNum();
  } while(!gameover());

  showBoard();
  printf("gameover...\n");
  printf("your score is: %d\n\n", score);

  // exit(0);
  end();
  return 0;
}
