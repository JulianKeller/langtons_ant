#include <MD_MAX72xx.h>
#include <SPI.h>

// display pins
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW   // Set display type  so that  MD_MAX72xx library treets it properly
#define MAX_DEVICES  4    // Total number display modules
#define CLK_PIN   13      // Clock pin to communicate with display
#define DATA_PIN  11      // Data pin to communicate with display
#define CS_PIN    3       // Control pin to communicate with display
#define DEBUG 0

// potentiometer
#define POT A2

// display dimensions
#define MAX_Y 8
#define MAX_X 32

// Define max iterations
#define MAX_ITERATIONS 30000

// define directions
#define N 1
#define E 2
#define S 3
#define W 4


int hasChanged = 1;
int iterations = 0;
int previousSum = 0;
int sum_count = 0;
int average = 0;
int previousAverage = 0;

// Initialize starting position
int ant_x = 16;
int ant_y = 4;
int dir = E;


// init game board so we can easily input a starting pattern
int board[MAX_Y][MAX_X] = {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

int board_aux[MAX_Y][MAX_X] = {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};


MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);   // display object

void setup() {
  mx.begin();             // initialize display
  mx.control(MD_MAX72XX::INTENSITY, 0);     // SET DISPLAY brightness
  Serial.begin(9600);     // open the serial port at 9600 bps:

  // get noise as long as analog 1 is not in use
  randomSeed(analogRead(1));

  init_langtons_ant();

}


void loop() {

  // enable for hard coded speed
  delay(50);
  displayBoard(board);
  langtonsAnt();

  // reset at max iterations
  if (iterations++ >= MAX_ITERATIONS) {
    init_langtons_ant();
    iterations = 0;
  }
  //  Serial.println(iterations);


}


void langtonsAnt() {

  int current_x = ant_x;
  int current_y = ant_y;

  // white cell
  if (board[ant_y][ant_x] == 0) {
    // change color of cell
    board[ant_y][ant_x] = 1;

    // determine next direction --> turn 90 degrees counter clockwise and move forward
    while (ant_x == current_x && ant_y == current_y) {
      switch (dir) {
        case N:
          dir = W;
          if (ant_x - 1 < 0) {
            dir = S;
          }
          else {
            ant_x--;
          }
          break;

        case E:
          dir = N;
          if (ant_y + 1 >= MAX_Y) {
            dir = W;
          }
          else {
            ant_y++;
          }
          break;

        case S:
          dir = E;
          if (ant_x + 1 >= MAX_X) {
            dir = N;
          }
          else {
            ant_x++;
          }
          break;

        case W:
          dir = S;
          if (ant_y - 1 < 0) {
            dir = E;
          }
          else {
            ant_y--;
          }
          break;
      }
    }
  }

  // black cell
  else {
    // change color of cell
    board[ant_y][ant_x] = 0;
    // determine next direction --> turn 90 degrees counter clockwise and move forward
    switch (dir) {
      case N:
        dir = E;
        if (ant_x + 1 >= MAX_X) {
          dir = S;
        }
        else {
          ant_x++;
        }
        break;
      case E:
        dir = S;
        if (ant_y - 1 < 0) {
          dir = W;
        }
        else {
          ant_y--;
        }
        break;
      case S:
        dir = W;
        if (ant_x - 1 < 0) {
          dir = N;
        }
        else {
          ant_x--;
        }
        break;
      case W:
        dir = N;
        if (ant_y + 1 >= MAX_Y) {
          dir = E;
        }
        else {
          ant_y++;
        }
        break;
    }

  }


}


// initialize the game board, the ant starting position, and starting direction
void init_langtons_ant() {
  random_init_board(board, 8);

  // get random starting x and y
  ant_y = random(0, MAX_Y);
  ant_x = random(0, MAX_X);

  // get random starting direction
  dir = random(1, 4);

  flashBoard(3);
}


// flash board on and off number of times
void flashBoard(int times) {
  int delay_time = 500;
  for (int i = 0; i < times; i++) {
    // clear the board with a flash
    displayBoard(board_aux);
    delay(delay_time);
    displayBoard(board);
    delay(delay_time);
  }
}



// copy the src board to the destination board
void copyBoard(int src[MAX_Y][MAX_X], int dest[MAX_Y][MAX_X]) {
  hasChanged = 0;
  for (int y = 0; y < MAX_Y; y++) {
    for (int x = 0; x < MAX_X; x++) {
      // detect if board has changed so it doesn't get stuck
      if (dest[y][x] != src[y][x]) {
        hasChanged = 1;
      }
      dest[y][x] = src[y][x];
    }
  }
}


int my_count_neighbors(int y, int x) {
  int count = 0;

  // above --------------------------------------------------------
  // above left
  if (y > 0 && x > 0) count += board[y - 1][x - 1];
  // above
  if (y > 0) count += board[y - 1][x];
  // above right
  if ((x + 1) < MAX_X && y > 0) count += board[y - 1][x + 1];

  // current row --------------------------------------------------
  // left
  if (x > 0) count += board[y][x - 1];
  // right
  if ((x + 1) < MAX_X) count += board[y][x + 1];

  // below --------------------------------------------------------
  // below left
  if ((y + 1) < MAX_Y && x > 0) count += board[y + 1][x - 1];
  // below
  if ((y + 1) < MAX_Y) count += board[y + 1][x];
  // below right
  if ((x + 1) < MAX_X && (y + 1) < MAX_Y) count += board[y + 1][x + 1];

  return count;
}


// TODO this almost works, but fails to return correct count
int getNeighbors(int y, int x) {
  int count = 0;
  // perp and vertical
  for (int j = -1; j < 2; j++) {
    for (int i = -1; i < 2; i++) {
      // don't count self
      if (i == 0 && j == 0) continue;
      // else count everything in bounds
      if ((y + j >= 0) && (y + j < MAX_Y) && (x + i >= 0) && (x + i < MAX_X)) {
        count += board[y + j][x + i];
      }
    }
  }
  return count;
}

void displayBoard(int the_board[MAX_Y][MAX_X]) {
  for (int y = 0; y < MAX_Y; y++) {
    for (int x = 0; x < MAX_X; x++) {
      mx.setPoint(y, x, the_board[y][x]);
    }
  }
}

// add up the total value of the board
int sumBoard() {
  int sum = 0;
  for (int y = 0; y < MAX_Y; y++) {
    for (int x = 0; x < MAX_X; x++) {
      sum += board[y][x];
    }
  }
  return sum;
}

void initBoard(int board[MAX_Y][MAX_X], int value) {
  for (int y = 0; y < MAX_Y; y++) {
    for (int x = 0; x < MAX_X; x++) {
      board[y][x] = value;
    }
  }
}

void resetBoard() {
  //  // clear the board with a flash
  ////    initBoard(board, 1);
  //    setBoardOutline(board);
  //    displayBoard(board);
  //    delay(100);
  //    initBoard(board, 0);
  //    displayBoard(board);

  // reset the board with 100 random points
  //  for (int i = 0; i < 10; i++) randomDisplay();

  set_random_point_near_neighbor(1, 3);

  //  set_random_point(1, 100);

  // set random board
  //    random_init_board(board, 100);
  sum_count = 0;
}



void setBoardOutline(int board[MAX_Y][MAX_X]) {
  for (int y = 0; y < MAX_Y; y++) {
    for (int x = 0; x < MAX_X; x++) {
      if (y == 0 || y == MAX_Y - 1) board[y][x] = 1;
      if (x == 0 || x == MAX_X - 1) board[y][x] = 1;
    }
  }
}

void random_init_board(int board[MAX_Y][MAX_X], int num_points) {
  int x = 0;
  int y = 0;
  for (int i = 0; i < num_points; i++) {
    y = random(0, MAX_Y);
    x = random(0, MAX_X);
    board[y][x] = 1;
  }
}


// standalone method that can be called in loop
void randomDisplay() {
  set_random_point(1, 1);
  set_random_point(0, 2);
  delay(100);
}


// repeate should be greater than the number of points to set
void set_random_point_near_neighbor(int state, int repeat) {
  int x = 0;
  int y = 0;
  int points = 4;

  int sum = sumBoard();

  // set a couple of random points if board is empty
  if (sum == 0) {
    for (int i = 0; i < points; i++) {
      y = random(0, MAX_Y);
      x = random(0, MAX_X);
      board[y][x] = state;
      mx.setPoint(y, x, state);
    }
    sum = points;
  }

  // set points only near neighbors
  int i = 0;
  int neighbors = 0;
  while (i < points * repeat) {
    y = random(0, MAX_Y);
    x = random(0, MAX_X);
    if (getNeighbors(y, x) > 0) {
      board[y][x] = state;
      mx.setPoint(y, x, state);
      i++;
      delay(50);
    }

  }
}

void set_random_point(int state, int repeat) {
  int x = 0;
  int y = 0;
  for (int i = 0; i < repeat; i++) {
    y = random(0, MAX_Y);
    x = random(0, MAX_X);
    board[y][x] = state;
    mx.setPoint(y, x, state);
  }
}

void printLine(char* str, int value) {
  if (DEBUG) {
    Serial.print(str);
    Serial.println(value);
  }
}


void printit(char* str, int value) {
  if (DEBUG) {
    Serial.print(str);
    Serial.print(value);
  }
}

void printant(int y, int x) {
  Serial.print(y);
  Serial.print(":");
  Serial.print(x);
  Serial.print(" = ");
  Serial.println(board[y][x]);

}

// debug board
void printBoard() {
  for (int y = 0; y < MAX_Y; y++) {
    for (int x = 0; x < MAX_X; x++) {
      Serial.print(board[y][x]);
      Serial.print(" ");
    }
    Serial.println("");
  }
  Serial.println("##########################################################################################");
}

void printBoardValues() {
  for (int y = 0; y < MAX_Y; y++) {
    for (int x = 0; x < MAX_X; x++) {
      Serial.print(y);
      Serial.print(":");
      Serial.print(x);
      Serial.print(" = ");
      Serial.println(board[y][x]);
    }
  }
}
