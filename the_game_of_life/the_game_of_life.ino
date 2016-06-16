/**
 * The Game of Life implementation.
 * Initial configurations were taken from https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life
 */
#include <LedControl.h>

#define CLK 8
#define CS  9
#define DIN 10

LedControl lc = LedControl(DIN, CLK, CS, 1);

int d[8][8] = {
  {-1, -1},
  {0, -1},
  {1, -1},
  {-1, 0},
  {1, 0},
  {-1, 1},
  {0, 1},
  {1, 1},
};

int glider[10][10] = {
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,1,0,0,0,0,0,0,0},
    {0,0,0,1,0,0,0,0,0,0},
    {0,1,1,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0} 
};

int LWSS[10][10] = {
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,1,1,0,0,0,0},
    {0,0,0,1,1,1,1,0,0,0},
    {0,0,1,1,0,1,1,0,0,0},
    {0,0,0,1,1,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0} 
};

int scene[10][10];

void setup() {
  lc.shutdown(0,false);
  lc.setIntensity(0,8);
}

int getNeighbours(int row, int col) {
  int n = 0;
  for (int k = 0; k < 8; k++) {
    n += scene[row + d[k][0]][col + d[k][1]];
  }
  return n;
}

void zeros(int a[][10], int rows) {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < 10; j++) {
      a[i][j] = 0;
    }
  }
}

void copy(int src[][10], int dst[][10]) {
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
      dst[i][j] = src[i][j];
    }
  }  
}

void evolve() {
  int next[10][10];
  zeros(&next[0], 10);

  //copy boundaries to make the scene infinite
  for (int i = 1; i <= 8; i++) {
    scene[i][9] = scene[i][1];
    scene[i][0] = scene[i][8];
    scene[9][i] = scene[1][i];
    scene[0][i] = scene[8][i];    
  }
  scene[0][0] = scene[8][8];
  scene[9][9] = scene[1][1];
  scene[0][9] = scene[8][1];
  scene[9][0] = scene[1][8];
  
  for (int i = 1; i <= 8; i++) {
    for (int j = 1; j <= 8; j++) {
      int n = getNeighbours(i, j);
      if (scene[i][j] == 1) {
        if (n < 2) next[i][j] = 0; //die from underpopulation
        else if (n == 2 || n == 3) next[i][j] = 1; //continue living
        else if (n > 3) next[i][j] = 0; // die from overpopulation        
      } else {
        if (n == 3) next[i][j] = 1; // reproduction        
      }
    }
  }

  copy(&next[0], &scene[0]);  
}

void render() {
  for (int i = 1; i <= 8; i++) {
    for (int j = 1; j <= 8; j++) {
      lc.setLed(0, i - 1, j - 1, scene[i][j]);
    }
  }  
}

int steps = 0;
int frame = 0;
int cycles = 0;

void loop() {
  if (steps == 0) {    
    switch (frame) {
      case 0: copy(&glider[0], &scene[0]); break;
      case 1: copy(&LWSS[0], &scene[0]); break;
      case 2: {
        randomSeed(cycles);
        for (int i = 0; i < 10; i++) {
          for (int j = 0; j < 10; j++) {
            scene[i][j] = random(2);
          }
        }
      }
    }
    steps = 50;
    cycles++;
    frame++;
    frame = frame % 3;
  }
  render();
  evolve();
  steps--;
  delay(200);
}
