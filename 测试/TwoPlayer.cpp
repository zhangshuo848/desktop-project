#include "TwoPlayer.h"

// Sprite 类实现
Sprite::Sprite(int x, int y, COLORREF color) : x(x), y(y), color(color) {}
void Sprite::moveBy(int dx, int dy) { x += dx; y += dy; }
int Sprite::getX() const { return x; }
int Sprite::getY() const { return y; }
void Sprite::draw() const {
    setfillcolor(color);
    solidrectangle(x, y, x + BLOCK_SIZE, y + BLOCK_SIZE);
}

// 蛇类实现
Snake::Snake(int startX, int startY, COLORREF color)
    : Sprite(startX, startY, color), dir(RIGHT), score(0) {
    body.emplace_back(startX, startY, color);
    body.emplace_back(startX - BLOCK_SIZE, startY, color);
    body.emplace_back(startX - 2 * BLOCK_SIZE, startY, color);
}

// ✅ 补全缺失函数1：设置初始方向
void Snake::SetInitialDirection(Direction d) {
    dir = d;
}

// ✅ 补全缺失函数2：获取分数
int Snake::GetScore() const {
    return score;
}

void Snake::draw() const {
    for (const auto& part : body) part.draw();
}

void Snake::Move() {
    for (int i = (int)body.size() - 1; i > 0; --i) {
        body[i] = body[i - 1];
    }
    switch (dir) {
    case UP:    body[0].moveBy(0, -BLOCK_SIZE); break;
    case DOWN:  body[0].moveBy(0, BLOCK_SIZE); break;
    case LEFT:  body[0].moveBy(-BLOCK_SIZE, 0); break;
    case RIGHT: body[0].moveBy(BLOCK_SIZE, 0); break;
    }
}

void Snake::ChangeDirection(Direction newDir) {
    if ((dir == UP && newDir == DOWN) ||
        (dir == DOWN && newDir == UP) ||
        (dir == LEFT && newDir == RIGHT) ||
        (dir == RIGHT && newDir == LEFT))
        return;
    dir = newDir;
}

bool Snake::checkSelfCollision() const {
    for (int i = 1; i < (int)body.size(); ++i) {
        if (body[0].getX() == body[i].getX() && body[0].getY() == body[i].getY())
            return true;
    }
    return false;
}

bool Snake::checkWallCollision() const {
    int x = body[0].getX();
    int y = body[0].getY();
    return x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT;
}

bool Snake::checkCollisionWith(const Snake& other) const {
    for (const auto& part : other.GetBody()) {
        if (body[0].getX() == part.getX() && body[0].getY() == part.getY())
            return true;
    }
    return false;
}

bool Snake::HeadHit(const Sprite& obj) const {
    return body[0].getX() == obj.getX() && body[0].getY() == obj.getY();
}

void Snake::Grow() {
    body.push_back(body.back());
    score += 10;
}

const std::vector<Sprite>& Snake::GetBody() const { return body; }

// 食物类实现
Food::Food() : Sprite(0, 0, YELLOW) {}

void Food::RandomPos(const Snake& s1, const Snake& s2) {
    bool valid;
    int nx, ny;
    do {
        valid = true;
        nx = rand() % (MAP_WIDTH / BLOCK_SIZE) * BLOCK_SIZE;
        ny = rand() % (MAP_HEIGHT / BLOCK_SIZE) * BLOCK_SIZE;

        for (const auto& p : s1.GetBody())
            if (p.getX() == nx && p.getY() == ny) valid = false;
        for (const auto& p : s2.GetBody())
            if (p.getX() == nx && p.getY() == ny) valid = false;

    } while (!valid);
    x = nx;
    y = ny;
}

// 游戏场景类实现
GameScene::GameScene()
    : snake1(200, MAP_HEIGHT / 2, GREEN),
    snake2(600, MAP_HEIGHT / 2, RED) {
    initgraph(MAP_WIDTH, MAP_HEIGHT);
    setbkcolor(BLACK);
    cleardevice();
    srand((unsigned)time(NULL));
    snake2.SetInitialDirection(LEFT);
    food.RandomPos(snake1, snake2);
    gameOver = false;
    winner = 0;
}

void GameScene::resetGame() {
    snake1 = Snake(200, MAP_HEIGHT / 2, GREEN);
    snake2 = Snake(600, MAP_HEIGHT / 2, RED);
    snake2.SetInitialDirection(LEFT);
    food.RandomPos(snake1, snake2);
    gameOver = false;
    winner = 0;
}

void GameScene::handleInput() {
    if (GetAsyncKeyState(VK_UP) & 0x8000)  snake1.ChangeDirection(UP);
    if (GetAsyncKeyState(VK_DOWN) & 0x8000)snake1.ChangeDirection(DOWN);
    if (GetAsyncKeyState(VK_LEFT) & 0x8000)snake1.ChangeDirection(LEFT);
    if (GetAsyncKeyState(VK_RIGHT) & 0x8000)snake1.ChangeDirection(RIGHT);

    if (GetAsyncKeyState('W') & 0x8000) snake2.ChangeDirection(UP);
    if (GetAsyncKeyState('S') & 0x8000) snake2.ChangeDirection(DOWN);
    if (GetAsyncKeyState('A') & 0x8000) snake2.ChangeDirection(LEFT);
    if (GetAsyncKeyState('D') & 0x8000) snake2.ChangeDirection(RIGHT);
}

void GameScene::checkCollisions() {
    bool p1Dead = snake1.checkWallCollision() || snake1.checkSelfCollision() || snake1.checkCollisionWith(snake2);
    bool p2Dead = snake2.checkWallCollision() || snake2.checkSelfCollision() || snake2.checkCollisionWith(snake1);

    if (p1Dead && p2Dead) { gameOver = true; winner = 0; }
    else if (p1Dead) { gameOver = true; winner = 2; }
    else if (p2Dead) { gameOver = true; winner = 1; }
}

void GameScene::endScreen() {
    while (true) {
        cleardevice();
        snake1.draw(); snake2.draw(); food.draw();

        settextcolor(RED);
        settextstyle(30, 0, _T("微软雅黑"));
        if (winner == 1) outtextxy(MAP_WIDTH / 2 - 100, MAP_HEIGHT / 2 - 50, _T("玩家1获胜！"));
        else if (winner == 2) outtextxy(MAP_WIDTH / 2 - 100, MAP_HEIGHT / 2 - 50, _T("玩家2获胜！"));
        else outtextxy(MAP_WIDTH / 2 - 100, MAP_HEIGHT / 2 - 50, _T("平局！"));

        outtextxy(MAP_WIDTH / 2 - 180, MAP_HEIGHT / 2 + 30, _T("按 R 重新开始 | 按 Q 退出"));

        if (GetAsyncKeyState('R') & 0x8000) { resetGame(); break; }
        if (GetAsyncKeyState('Q') & 0x8000) { closegraph(); exit(0); }
        Sleep(100);
    }
}

void GameScene::run() {
    while (true) {
        if (gameOver) { endScreen(); continue; }

        BeginBatchDraw();
        cleardevice();
        handleInput();

        snake1.Move();
        snake2.Move();

        if (snake1.HeadHit(food)) { snake1.Grow(); food.RandomPos(snake1, snake2); }
        if (snake2.HeadHit(food)) { snake2.Grow(); food.RandomPos(snake1, snake2); }

        checkCollisions();

        snake1.draw();
        snake2.draw();
        food.draw();

        TCHAR s1[50], s2[50];
        _stprintf_s(s1, _T("P1: %d"), snake1.GetScore());
        _stprintf_s(s2, _T("P2: %d"), snake2.GetScore());

        settextcolor(WHITE);
        outtextxy(20, 20, s1);
        outtextxy(MAP_WIDTH - 100, 20, s2);

        EndBatchDraw();
        Sleep(150);
    }
}