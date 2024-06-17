#include <curses.h>
#include <chrono>
#include <thread>
#include <deque>
#include <cstdlib>
#include <ctime>
#include <iostream>

const int WIDTH = 40;
const int HEIGHT = 20;
const int ITEM_LIFETIME = 5; 
const int GAME_SPEED = 200;

int gate1X, gate1Y;
int gate2X, gate2Y;

char map[HEIGHT][WIDTH];

struct Snake {
    int x, y; 
    std::deque<std::pair<int, int>> body;
    int dx, dy; 
    bool growFlag;

    Snake(int startX, int startY) : x(startX), y(startY), dx(0), dy(0), growFlag(false) {
        body.push_front(std::make_pair(startX, startY));
    }

    void move() {
        if (dx == 0 && dy == 0) return;

        int newX = x + dx;
        int newY = y + dy;

        if (map[newY][newX] == '7') {
            newX = gate2X;
            newY = gate2Y;
        }
        else if (map[newY][newX] == '8') {
            newX = gate1X;
            newY = gate1Y;
        }

        if (newX <= 0 || newX >= WIDTH - 1 || newY <= 0 || newY >= HEIGHT - 1 || map[newY][newX] == '#' || map[newY][newX] == '@') {
            endwin();
            std::cerr << "게임 오버: 벽이나 게이트에 부딪혔습니다!" << std::endl;
            exit(0);
        }

        for (auto& segment : body) {
            if (newX == segment.first && newY == segment.second) {
                endwin();
                std::cerr << "게임 오버: 자기 자신에게 부딪혔습니다!" << std::endl;
                exit(0);
            }
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
    init_pair(6, COLOR_WHITE, COLOR_BLACK);   

    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            if (map[i][j] == '#') {
                attron(COLOR_PAIR(1));
                mvaddch(i, j, ACS_CKBOARD);
                attroff(COLOR_PAIR(1));
            }
            else if (map[i][j] == '@') {
                attron(COLOR_PAIR(6));
                mvaddch(i, j, ACS_CKBOARD);
                attroff(COLOR_PAIR(6));
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
            else if (map[i][j] == '7' || map[i][j] == '8') {

                attron(COLOR_PAIR(5));
                mvaddch(i, j, ACS_CKBOARD);
                attroff(COLOR_PAIR(5));
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
    attroff(COLOR_PAIR(1));

    mvprintw(9, WIDTH + 5, "B: 10 (%s)", snake.length() >= 10 ? "v" : " ");
    mvprintw(10, WIDTH + 5, "+: 5 (%s)", growthItemCount >= 5 ? "v" : " ");
    mvprintw(11, WIDTH + 5, "-: 2 (%s)", poisonItemCount >= 2 ? "v" : " ");
    mvprintw(12, WIDTH + 5, "G: 1 (%s)", gateUsageCount > 0 ? "v" : " ");

    refresh();
}

void placeItem(char item) {
    int x, y;
    do {
        x = rand() % (WIDTH - 2) + 1;
        y = rand() % (HEIGHT - 2) + 1;
    } while (map[y][x] != ' ');

    map[y][x] = item;
}

void placeGate() {
    int x1, y1, x2, y2;

    do {
        x1 = rand() % (WIDTH - 2) + 1;
        y1 = rand() % (HEIGHT - 2) + 1;
    } while (map[y1][x1] != ' ' && map[y1][x1] != '#'); 

    map[y1][x1] = '7'; 
    gate1X = x1;
    gate1Y = y1;

    do {
        x2 = rand() % (WIDTH - 2) + 1;
        y2 = rand() % (HEIGHT - 2) + 1;
    } while ((map[y2][x2] != ' ' && map[y2][x2] != '#') || (x1 == x2 && y1 == y2)); 

    map[y2][x2] = '8'; 
    gate2X = x2;
    gate2Y = y2;
}

int main() {
    initscr();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    timeout(GAME_SPEED); 
    noecho();
    curs_set(0);
    srand(time(NULL));

    Snake snake(WIDTH / 2, HEIGHT / 2);
    int growthItemCount = 0;
    int poisonItemCount = 0;
    int gateUsageCount = 0;
    int maxLength = 0;
    int secondsElapsed = 0;
    auto startTime = std::chrono::steady_clock::now();
    auto lastItemTime = startTime;
    auto lastGateTime = startTime; 

    initializeMap();

    for (int i = 0; i < WIDTH; ++i) {
        map[0][i] = '#';
        map[HEIGHT - 1][i] = '#';
    }
    for (int i = 0; i < HEIGHT; ++i) {
        map[i][0] = '#';
        map[i][WIDTH - 1] = '#';
    }

    map[5][5] = '@';
    map[5][6] = '@';
    map[5][7] = '@';

    placeItem('5');
    placeItem('6');

    int input;
    while (true) {
        input = getch();
        switch (input) {
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
        }

        auto now = std::chrono::steady_clock::now();
        secondsElapsed = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();

        if (std::chrono::duration_cast<std::chrono::seconds>(now - lastItemTime).count() >= ITEM_LIFETIME) {
            for (int i = 1; i < HEIGHT - 1; ++i) {
                for (int j = 1; j < WIDTH - 1; ++j) {
                    if (map[i][j] == '5' || map[i][j] == '6') {
                        map[i][j] = ' ';
                    }
                }
            }
            placeItem('5');
            placeItem('6');
            lastItemTime = now;
        }

        if (std::chrono::duration_cast<std::chrono::seconds>(now - lastGateTime).count() >= 10 || snake.length() > 10) {
            for (int i = 1; i < HEIGHT - 1; ++i) {
                for (int j = 1; j < WIDTH - 1; ++j) {
                    if (map[i][j] == '7' || map[i][j] == '8') {
                        map[i][j] = ' ';
                    }
                }
            }
            placeGate();
            lastGateTime = now;
        }

        snake.move();

        if (map[snake.y][snake.x] == '5') {
            snake.grow();
            map[snake.y][snake.x] = ' ';
            growthItemCount++;
        }
        else if (map[snake.y][snake.x] == '6') {
            if (snake.shrink()) {
                poisonItemCount++;
            }
            map[snake.y][snake.x] = ' ';
        }

        if (snake.length() > maxLength) {
            maxLength = snake.length();
        }

        if (map[snake.y][snake.x] == '7' || map[snake.y][snake.x] == '8') {
            gateUsageCount++;
        }

        drawMap(snake, maxLength, growthItemCount, poisonItemCount, gateUsageCount, secondsElapsed);

        std::this_thread::sleep_for(std::chrono::milliseconds(GAME_SPEED));
    }

    endwin();
    return 0;
}
