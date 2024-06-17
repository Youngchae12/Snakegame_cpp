#include <curses.h>
#include <chrono>
#include <thread>
#include <deque>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <algorithm>

const int WIDTH = 40;
const int HEIGHT = 20;

int gate1X, gate1Y;
int gate2X, gate2Y;

char map[HEIGHT][WIDTH];

struct Snake {
    int x, y; 
    std::deque<std::pair<int, int>> body;
    int dx, dy; 
    bool growFlag;

    Snake(int startX, int startY) : x(startX), y(startY), dx(1), dy(0), growFlag(false) {
        body.push_front(std::make_pair(startX, startY));
    }

    void move() {
        int newX = x + dx;
        int newY = y + dy;

        if (map[newY][newX] == '7') {
            newX = gate2X;
            newY = gate2Y;
            setDirectionForGate(gate2X, gate2Y);
        }
        else if (map[newY][newX] == '8') {
            newX = gate1X;
            newY = gate1Y;
            setDirectionForGate(gate1X, gate1Y);
        }

        for (auto& segment : body) {
            if (newX == segment.first && newY == segment.second) {
                endwin();
                std::cerr << "게임 오버: 자기 자신에게 부딪혔습니다!" << std::endl;
                exit(0);
            }
        }

        if (map[newY][newX] == '#') {
            endwin();
            std::cerr << "게임 오버: 벽에 부딪혔습니다!" << std::endl;
            exit(0);
        }

        x = newX;
        y = newY;
        body.push_front(std::make_pair(x, y));

        if (!growFlag) {
            body.pop_back();
        }
        else {
            growFlag = false;
        }
    }

    void setDirection(int newDx, int newDy) {
        if (body.size() <= 1 || (dx + newDx != 0 && dy + newDy != 0)) {
            dx = newDx;
            dy = newDy;
        }
    }

    void setDirectionForGate(int gateX, int gateY) {
        if (gateY == 0) {
            setDirection(0, 1); 
        }
        else if (gateY == HEIGHT - 1) {
            setDirection(0, -1); 
        }
        else if (gateX == 0) {
            setDirection(1, 0); 
        }
        else if (gateX == WIDTH - 1) {
            setDirection(-1, 0); 
        }
    }

    void grow() {
        growFlag = true;
    }

    bool shrink() {
        if (body.size() > 1) {
            body.pop_back();
            return true;
        }
        return false;
    }

    int length() const {
        return body.size();
    }
};

void initializeMap() {
    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            map[i][j] = ' ';
        }
    }
}

void drawMap(Snake& snake, int maxLength, int growthItemCount, int poisonItemCount, int gateUsageCount, int secondsElapsed) {
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK); 
    init_pair(2, COLOR_RED, COLOR_BLACK); 
    init_pair(3, COLOR_YELLOW, COLOR_BLACK); 
    init_pair(4, COLOR_BLUE, COLOR_BLACK); 
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK); 
    init_pair(6, COLOR_MAGENTA, COLOR_BLACK); 

    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            if (map[i][j] == '#') {
                attron(COLOR_PAIR(1));
                mvaddch(i, j, ACS_CKBOARD);
                attroff(COLOR_PAIR(1));
            }
            else if (map[i][j] == 'O') {
                attron(COLOR_PAIR(2));
                mvaddch(i, j, ACS_CKBOARD);
                attroff(COLOR_PAIR(2));
            }
            else if (map[i][j] == '5') {
                attron(COLOR_PAIR(3));
                mvaddch(i, j, ACS_DIAMOND);
                attroff(COLOR_PAIR(3));
            }
            else if (map[i][j] == '6') {
                attron(COLOR_PAIR(4));
                mvaddch(i, j, ACS_DIAMOND);
                attroff(COLOR_PAIR(4));
            }
            else if (map[i][j] == '7') {
                attron(COLOR_PAIR(5));
                mvaddch(i, j, ACS_DIAMOND);
                attroff(COLOR_PAIR(5));
            }
            else if (map[i][j] == '8') {
                attron(COLOR_PAIR(6));
                mvaddch(i, j, ACS_DIAMOND);
                attroff(COLOR_PAIR(6));
            }
            else {
                mvaddch(i, j, map[i][j]);
            }
        }
    }


    attron(COLOR_PAIR(2));
    for (auto& segment : snake.body) {
        mvaddch(segment.second, segment.first, ACS_CKBOARD);
    }
    attroff(COLOR_PAIR(2));


    attron(COLOR_PAIR(1));
    mvprintw(1, WIDTH + 5, "Score Board");
    mvprintw(2, WIDTH + 5, "B: %d / %d", snake.length(), maxLength);
    mvprintw(3, WIDTH + 5, "+: %d", growthItemCount);
    mvprintw(4, WIDTH + 5, "-: %d", poisonItemCount);
    mvprintw(5, WIDTH + 5, "G: %d", gateUsageCount);
    mvprintw(6, WIDTH + 5, "Time: %ds", secondsElapsed);
    attroff(COLOR_PAIR(1));


    attron(COLOR_PAIR(1));
    mvprintw(8, WIDTH + 5, "Mission");
    mvprintw(9, WIDTH + 5, "B: 10 (%s)", snake.length() >= 10 ? "v" : " ");
    mvprintw(10, WIDTH + 5, "+: 3 (%s)", growthItemCount >= 3 ? "v" : " ");
    mvprintw(11, WIDTH + 5, "-: 2 (%s)", poisonItemCount >= 2 ? "v" : " ");
    mvprintw(12, WIDTH + 5, "G: 1 (%s)", gateUsageCount >= 1 ? "v" : " ");
    attroff(COLOR_PAIR(1));
}

void placeWalls() {
    for (int i = 0; i < HEIGHT; ++i) {
        map[i][0] = '#';
        map[i][WIDTH - 1] = '#';
    }
    for (int j = 0; j < WIDTH; ++j) {
        map[0][j] = '#';
        map[HEIGHT - 1][j] = '#';
    }
}

void placeItem(char item) {
    int x, y;
    do {
        x = rand() % WIDTH;
        y = rand() % HEIGHT;
    } while (map[y][x] != ' ');

    map[y][x] = item;
}

void placeGate() {
    int x1, y1, x2, y2;
    do {
        x1 = rand() % WIDTH;
        y1 = rand() % HEIGHT;
    } while (map[y1][x1] != '#');

    do {
        x2 = rand() % WIDTH;
        y2 = rand() % HEIGHT;
    } while ((x2 == x1 && y2 == y1) || map[y2][x2] != '#');

    gate1X = x1;
    gate1Y = y1;
    gate2X = x2;
    gate2Y = y2;

    map[gate1Y][gate1X] = '7'; 
    map[gate2Y][gate2X] = '8'; 
}

int main() {
    srand(time(0));
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    timeout(100); 

    Snake snake(WIDTH / 2, HEIGHT / 2);

    initializeMap();
    placeWalls();
    placeGate();
    placeItem('5'); 
    placeItem('6'); 

    int growthItemCount = 0;
    int poisonItemCount = 0;
    int gateUsageCount = 0;
    int maxLength = 0;
    int startTime = time(0);

    while (true) {
        int ch = getch();
        switch (ch) {
        case KEY_UP:
            snake.setDirection(0, -1);
            break;
        case KEY_DOWN:
            snake.setDirection(0, 1);
            break;
        case KEY_LEFT:
            snake.setDirection(-1, 0);
            break;
        case KEY_RIGHT:
            snake.setDirection(1, 0);
            break;
        case 'q':
            endwin();
            return 0;
        }

        snake.move();

        if (map[snake.y][snake.x] == '5') { 
            snake.grow();
            placeItem(' '); 
            growthItemCount++;
            placeItem('5'); 
        }
        else if (map[snake.y][snake.x] == '6') { 
            if (!snake.shrink()) {
                endwin();
                std::cerr << "Game Over: Snake is too small!" << std::endl;
                return 0;
            }
            placeItem(' '); 
            poisonItemCount++;
            placeItem('6'); 
        }
        else if (map[snake.y][snake.x] == '7' || map[snake.y][snake.x] == '8') { 
            gateUsageCount++;
          
        }

        if (snake.length() > maxLength) {
            maxLength = snake.length();
        }

        clear();
        int currentTime = time(0);
        int secondsElapsed = currentTime - startTime;
        drawMap(snake, maxLength, growthItemCount, poisonItemCount, gateUsageCount, secondsElapsed);
        refresh();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    endwin();
    return 0;
}
