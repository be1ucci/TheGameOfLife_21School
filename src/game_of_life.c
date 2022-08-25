#include <ncurses.h>
#include <stdio.h>
#include <unistd.h>

#define N 25
#define M 80

void draw_Everything(int cur_State[N][M]);
void state_Calc(int cur_State[N][M], int next_State[N][M]);
void cell_State(int cur_State[N][M], int next_State[N][M], int row, int column);
void first_Pattern(int cur_State[N][M]);
void key_Check(int *t, int *global_Flag);

int main() {
  int cur_State[N][M] = {0};
  int next_State[N][M] = {0};

  first_Pattern(cur_State);
  int t = 100000;
  int global_Flag = 1;
  initscr();
  FILE *tty;
  tty = freopen("/dev/tty", "r", stdin);
  if (tty == NULL)
    global_Flag = 0;
  while (global_Flag) {
    nodelay(stdscr, TRUE);
    noecho();
    draw_Everything(cur_State);
    state_Calc(cur_State, next_State);
    key_Check(&t, &global_Flag);
  }
  nodelay(stdscr, FALSE);
  endwin();
  return 0;
}

void key_Check(int *t, int *global_Flag) {
  usleep(*t);
  int x = getch();
  if ((x == '.' || x == '>') && *t >= 10000) {
    *t /= 2;
  } else if ((x == ',' || x == '<') && *t <= 300000) {
    *t *= 2;
  } else if (x == 'q' || x == 'Q') {
    *global_Flag = 0;
  }
}

void first_Pattern(int cur_State[N][M]) {
  int flag = 1;
  FILE *fp;
  do {
    char *patt = "pattern";
    char *ext = ".txt";
    char num;

    printf("Введите номер паттерна (1-5):\n");
    scanf("%c", &num);
    char file_Name[13];
    snprintf(file_Name, sizeof file_Name, "%s%c%s", patt, num, ext);
    if ((fp = fopen(file_Name, "r")) != NULL) {
      flag = 0;
    }
    printf("\033[0d\033[2J");
  } while (flag);
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < M; j++) {
      fscanf(fp, "%d", &cur_State[i][j]);
    }
  }
  fclose(fp);
}

void draw_Everything(int cur_State[N][M]) {
  clear();
  for (int i = -5; i <= N; i++) {
    for (int j = -1; j <= M; j++) {
      if (i == -1 || i == N) {
        printw("-");
      } else if (i == -3 && j == 1) {
        printw("q - Exit game\t\t< - Decrease speed\t\t> - Increase speed");
      } else if (i == -5 && j == 27) {
        printw("GAME OF LIFE (BETA)");
      } else if ((j == -1 || j == M) && i >= 0) {
        printw("|");
      } else if (i > -1 && cur_State[i][j] == 0) {
        addch(' ' | A_BLINK | A_BOLD);
      } else if (i > -1 && cur_State[i][j] == 1) {
        addch('*' | A_BLINK | A_BOLD);
      } else {
        printw(" ");
      }
    }
    printw("\n");
  }
  refresh();
}

void state_Calc(int cur_State[N][M], int next_State[N][M]) {
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < M; j++) {
      cell_State(cur_State, next_State, i, j);
    }
  }

  for (int i = 0; i < N; i++) {
    for (int j = 0; j < M; j++) {
      cur_State[i][j] = next_State[i][j];
    }
  }
}

void cell_State(int cur_State[N][M], int next_State[N][M], int row,
                int column) {
  int sum = -cur_State[row][column];
  int r = row - 1;
  for (int i = (r + N) % N; r < row + 2; i = (++r + N) % N) {
    int c = column - 1;
    for (int j = (c + M) % M; c < column + 2; j = (++c + M) % M) {
      sum += cur_State[i][j];
    }
  }
  if (cur_State[row][column] == 0 && sum == 3) {
    next_State[row][column] = 1;
  } else if (cur_State[row][column] == 1 && (sum < 2 || sum > 3)) {
    next_State[row][column] = 0;
  } else {
    next_State[row][column] = cur_State[row][column];
  }
}
