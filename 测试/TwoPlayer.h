#ifndef TWOPLAYER_H
#define TWOPLAYER_H

#include <easyx.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cstdio>

const int MAP_WIDTH = 800;
const int MAP_HEIGHT = 600;
const int BLOCK_SIZE = 20;

enum Direction {
    UP, DOWN, LEFT, RIGHT
};

class Sprite {
protected:
    int x, y;
    COLORREF color;
public:
    Sprite(int x = 0, int y = 0, COLORREF color = WHITE);
    virtual void draw() const;
    void moveBy(int dx, int dy);
    int getX() const;
    int getY() const;
};

class Snake : public Sprite {
private:
    std::vector<Sprite> body;
    Direction dir;
    int score;
public:
    Snake(int startX, int startY, COLORREF color);
    void draw() const override;
    void Move();
    void ChangeDirection(Direction newDir);
    void SetInitialDirection(Direction d);
    bool checkSelfCollision() const;
    bool checkWallCollision() const;
    bool checkCollisionWith(const Snake& other) const;
    bool HeadHit(const Sprite& obj) const;
    void Grow();
    int GetScore() const;
    const std::vector<Sprite>& GetBody() const;
};

class Food : public Sprite {
public:
    Food();
    void RandomPos(const Snake& s1, const Snake& s2);
};

class GameScene {
private:
    Snake snake1;
    Snake snake2;
    Food food;
    bool gameOver;
    int winner;

    void handleInput();
    void checkCollisions();
    void endScreen();
public:
    GameScene();
    void resetGame();
    void run();
};

#endif