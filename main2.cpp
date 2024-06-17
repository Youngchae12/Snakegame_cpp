#include <curses.h>
#include <chrono>
#include <thread>
#include <deque>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <algorithm>
#include <vector>

const int WIDTH = 40;
const int HEIGHT = 20;
const int ITEM_LIFETIME = 5;
const int MISSION_TIME_LIMIT = 1000;

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

        if (body.size() > 1 && ((dx != 0 && newX == body[1].first) || (dy != 0 && newY == body[1].second))) {
            return;
        }

        if (newX <= 0 || newX >= WIDTH - 1 || newY <= 0 || newY >= HEIGHT - 1) {
            endwin();
            std::cerr << "Game Over: Hit the wall!" << std::endl;
            exit(0);
        }

        for (auto& segment : body) {
            if (newX == segment.first && newY == segment.second) {
                endwin();
                std::cerr << "Game Over: Ran into yourself!" << std::endl;
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

char map[HEIGHT][WIDTH];

void initializeMap() {
    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            map[i][j] = ' ';
        }
    }
}

void drawMap(Snake& snake, int maxLength, int growthItemCount, int poisonItemCount, int secondsElapsed) {
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);

    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            switch (map[i][j]) {
            case '#':
                attron(COLOR_PAIR(1));
                mvaddch(i, j, ACS_CKBOARD);
                attroff(COLOR_PAIR(1));
                break;
            case 'O':
                attron(COLOR_PAIR(3));
                mvaddch(i, j, ACS_DIAMOND);
                attroff(COLOR_PAIR(3));
                break;
            case 'X':
                attron(COLOR_PAIR(4));
                mvaddch(i, j, ACS_DIAMOND);
                attroff(COLOR_PAIR(4));
                break;
            default:
                mvaddch(i, j, map[i][j]);
                break;
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
    mvprintw(5, WIDTH + 5, "Time: %ds", secondsElapsed);
    attroff(COLOR_PAIR(1));

    attron(COLOR_PAIR(1));
    mvprintw(7, WIDTH + 5, "Mission");
    attroff(COLOR_PAIR(1));

    mvprintw(8, WIDTH + 5, "B: 10 (%s)", snake.length() >= 10 ? "v" : " ");
    mvprintw(9, WIDTH + 5, "+: 3 (%s)", growthItemCount >= 3 ? "v" : " ");
    mvprintw(10, WIDTH + 5, "-: 2 (%s)", poisonItemCount >= 2 ? "v" : " ");

    refresh();
}

void placeItem(char item, std::chrono::steady_clock::time_point& lastItemTime) {
    int x, y;
    do {
        x = rand() % (WIDTH - 2) + 1;
        y = rand() % (HEIGHT - 2) + 1;
    } while (map[y][x] != ' ');

    map[y][x] = item;
    lastItemTime = std::chrono::steady_clock::now();
}

void removeItem(char item) {
    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            if (map[i][j] == item) {
                map[i][j] = ' ';
            }
        }
    }
}

int main() {
    srand(time(0));
    initscr();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);

    initializeMap();

    for (int i = 0; i < HEIGHT; ++i) {
        map[i][0] = '#';
        map[i][WIDTH - 1] = '#';
    }
    for (int j = 0; j < WIDTH; ++j) {
        map[0][j] = '#';
        map[HEIGHT - 1][j] = '#';
    }

    Snake snake(WIDTH / 2, HEIGHT / 2);

    auto lastItemTime = std::chrono::steady_clock::now();
    int maxLength = 1;
    int growthItemCount = 0;
    int poisonItemCount = 0;
    int secondsElapsed = 0;

    placeItem('O', lastItemTime);
    placeItem('X', lastItemTime);

    while (true) {
        clear();
        drawMap(snake, maxLength, growthItemCount, poisonItemCount, secondsElapsed);

        int input = getch();
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
        default:
            break;
        }

        snake.move();

        auto currentTime = std::chrono::steady_clock::now();
        int elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastItemTime).count();
        if (elapsedTime >= ITEM_LIFETIME) {
            removeItem('O');
            removeItem('X');
            placeItem('O', lastItemTime);
            placeItem('X', lastItemTime);
        }

        switch (map[snake.y][snake.x]) {
        case 'O':
            snake.grow();
            growthItemCount++;
            removeItem('O');
            placeItem('O', lastItemTime);
            break;
        case 'X':
            if (snake.shrink()) {
                poisonItemCount++;
            }
            removeItem('X');
            placeItem('X', lastItemTime);
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        secondsElapsed++;

        refresh();

        if (growthItemCount >= 3 || poisonItemCount >= 3) { 
            endwin();
            std::cout << "Congratulations! Mission completed!" << std::endl;

            std::cout << "다음 단계로 이동하세요. 아무 버튼을 누르면 다음 단계로 이동합니다." << std::endl;
            std::cin.get(); 

            return 0;
        }

        if (secondsElapsed > MISSION_TIME_LIMIT) {
            endwin();
            std::cerr << "Mission failed: Time limit exceeded!" << std::endl;
            return 0;
        }
    }

    endwin();
    return 0;
}
