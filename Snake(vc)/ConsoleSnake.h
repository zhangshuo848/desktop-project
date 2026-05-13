#pragma once
#include <iostream>
#include <vector>
#include <conio.h>
#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <string>

// 地图大小（字符格数）
const int WIDTH  = 40;
const int HEIGHT = 20;

// 方向
enum Dir { UP = 0, DOWN = 1, LEFT = 2, RIGHT = 3 };

// 坐标
struct Point {
    int x, y;
    Point(int _x = 0, int _y = 0) : x(_x), y(_y) {}
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

// 蛇
class Snake {
public:
    std::vector<Point> body;  // body[0] 是头
    Dir direction;
    int score;

    Snake(Point start, Dir dir);
    void move();
    void grow();
    bool checkSelfCollision() const;
    bool checkWallCollision() const;
};

// 食物
class Food {
public:
    Point pos;
    void randomPos(const Snake& s1, const Snake& s2);
};

// 游戏类
class Game {
public:
    Game();
    void run();

private:
    Snake player1, player2;
    Food food;
    bool gameOver;
    int winner;  // 0=进行中, 1=P1胜, 2=P2胜, 3=平局

    void draw() const;
    void input();
    void update();
    void endScreen();
    void reset();
    void gotoxy(int x, int y) const;   // 移动光标
    void hideCursor() const;
};