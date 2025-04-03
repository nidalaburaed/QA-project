#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <ncurses.h>
#include <unistd.h>

using namespace std;

// Directions
enum Direction { UP, DOWN, LEFT, RIGHT };

// Worm segment structure
struct Position {
    int x, y;
};

class WormGame {
private:
    int width, height;
    vector<Position> worm;
    Position food;
    Direction dir;
    bool gameOver;

public:
    WormGame(int w, int h) : width(w), height(h), gameOver(false) {
        initscr();
        clear();
        noecho();
        cbreak();
        curs_set(0);
        keypad(stdscr, TRUE);
        nodelay(stdscr, TRUE);
        srand(time(0));

        // Initialize worm in the center
        worm.push_back({width / 2, height / 2});
        dir = RIGHT;
        spawnFood();
    }

    ~WormGame() {
        endwin();
    }

    void spawnFood() {
        food.x = rand() % (width - 2) + 1;
        food.y = rand() % (height - 2) + 1;
    }

    void processInput() {
        int ch = getch();
        switch (ch) {
            case KEY_UP:    if (dir != DOWN) dir = UP; break;
            case KEY_DOWN:  if (dir != UP) dir = DOWN; break;
            case KEY_LEFT:  if (dir != RIGHT) dir = LEFT; break;
            case KEY_RIGHT: if (dir != LEFT) dir = RIGHT; break;
            case 'q': gameOver = true; break; // Quit key
        }
    }

    void update() {
        Position newHead = worm.front();

        // Move in the direction
        switch (dir) {
            case UP:    newHead.y--; break;
            case DOWN:  newHead.y++; break;
            case LEFT:  newHead.x--; break;
            case RIGHT: newHead.x++; break;
        }

        // Collision detection
        if (newHead.x <= 0 || newHead.x >= width - 1 ||
            newHead.y <= 0 || newHead.y >= height - 1) {
            gameOver = true;
        }

        // Check if worm bites itself
        for (auto &segment : worm) {
            if (newHead.x == segment.x && newHead.y == segment.y) {
                gameOver = true;
                return;
            }
        }

        // Add new head
        worm.insert(worm.begin(), newHead);

        // If food is eaten
        if (newHead.x == food.x && newHead.y == food.y) {
            spawnFood(); // Generate new food
        } else {
            worm.pop_back(); // Remove tail if no food eaten
        }
    }

    void draw() {
        clear();

        // Draw borders
        for (int i = 0; i < width; i++) mvprintw(0, i, "#");
        for (int i = 0; i < width; i++) mvprintw(height - 1, i, "#");
        for (int i = 0; i < height; i++) mvprintw(i, 0, "#");
        for (int i = 0; i < height; i++) mvprintw(i, width - 1, "#");

        // Draw worm
        for (size_t i = 0; i < worm.size(); i++) {
            mvprintw(worm[i].y, worm[i].x, (i == 0) ? "O" : "o");
        }

        // Draw food
        mvprintw(food.y, food.x, "*");

        refresh();
    }

    bool isGameOver() const {
        return gameOver;
    }
};

int main() {
    int width = 30, height = 15;
    WormGame game(width, height);

    while (!game.isGameOver()) {
        game.processInput();
        game.update();
        game.draw();
        usleep(100000); // Control speed
    }

    // Game over message
    clear();
    mvprintw(height / 2, width / 2 - 5, "Game Over!");
    refresh();
    sleep(2);

    return 0;
}