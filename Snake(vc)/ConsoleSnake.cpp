#include "ConsoleSnake.h"

// ========== Snake 实现 ==========
Snake::Snake(Point start, Dir dir) : direction(dir), score(0) {
    // 初始化身体：头 + 两节
    body.push_back(start);
    Point p2 = start, p3 = start;
    switch (dir) {
    case RIGHT: p2.x--; p3.x -= 2; break;
    case LEFT:  p2.x++; p3.x += 2; break;
    case UP:    p2.y++; p3.y += 2; break;
    case DOWN:  p2.y--; p3.y -= 2; break;
    }
    body.push_back(p2);
    body.push_back(p3);
}

void Snake::move() {
    // 身体从尾到头前移
    for (size_t i = body.size() - 1; i > 0; --i)
        body[i] = body[i - 1];
    // 头移动
    switch (direction) {
    case UP:    body[0].y--; break;
    case DOWN:  body[0].y++; break;
    case LEFT:  body[0].x--; break;
    case RIGHT: body[0].x++; break;
    }
}

void Snake::grow() {
    body.push_back(body.back());
    score += 10;
}

bool Snake::checkSelfCollision() const {
    for (size_t i = 1; i < body.size(); ++i)
        if (body[0] == body[i]) return true;
    return false;
}

bool Snake::checkWallCollision() const {
    return body[0].x <= 0 || body[0].x >= WIDTH - 1 ||
           body[0].y <= 0 || body[0].y >= HEIGHT - 1;
}

// ========== Food 实现 ==========
void Food::randomPos(const Snake& s1, const Snake& s2) {
    bool ok;
    do {
        ok = true;
        pos.x = rand() % (WIDTH - 2) + 1;
        pos.y = rand() % (HEIGHT - 2) + 1;
        // 避开蛇1
        for (auto& p : s1.body)
            if (p == pos) { ok = false; break; }
        // 避开蛇2
        if (ok)
            for (auto& p : s2.body)
                if (p == pos) { ok = false; break; }
    } while (!ok);
}

// ========== Game 实现 ==========
Game::Game() : player1(Point(WIDTH / 2 - 5, HEIGHT / 2), RIGHT),
               player2(Point(WIDTH / 2 + 5, HEIGHT / 2), LEFT),
               gameOver(false), winner(0) {
    srand((unsigned)time(nullptr));
    food.randomPos(player1, player2);
    hideCursor();
}

void Game::run() {
    while (true) {
        if (!gameOver) {
            input();
            update();
        }
        draw();
        if (gameOver) endScreen();
        Sleep(50);  // 控制速度
    }
}

void Game::input() {
    // 同时检测两个玩家的按键（需要底层键盘检测）
    // 这里使用 GetAsyncKeyState，适合 Windows 控制台
    if (GetAsyncKeyState(VK_UP)    & 0x8000) {
        if (player1.direction != DOWN) player1.direction = UP;
    }
    else if (GetAsyncKeyState(VK_DOWN)  & 0x8000) {
        if (player1.direction != UP) player1.direction = DOWN;
    }
    else if (GetAsyncKeyState(VK_LEFT)  & 0x8000) {
        if (player1.direction != RIGHT) player1.direction = LEFT;
    }
    else if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
        if (player1.direction != LEFT) player1.direction = RIGHT;
    }

    if (GetAsyncKeyState('W') & 0x8000) {
        if (player2.direction != DOWN) player2.direction = UP;
    }
    else if (GetAsyncKeyState('S') & 0x8000) {
        if (player2.direction != UP) player2.direction = DOWN;
    }
    else if (GetAsyncKeyState('A') & 0x8000) {
        if (player2.direction != RIGHT) player2.direction = LEFT;
    }
    else if (GetAsyncKeyState('D') & 0x8000) {
        if (player2.direction != LEFT) player2.direction = RIGHT;
    }
}

void Game::update() {
    // 移动两条蛇
    player1.move();
    player2.move();

    // 吃食物检测
    if (player1.body[0] == food.pos) {
        player1.grow();
        food.randomPos(player1, player2);
    }
    if (player2.body[0] == food.pos) {
        player2.grow();
        food.randomPos(player1, player2);
    }

    // 碰撞检测
    bool p1Dead = player1.checkWallCollision() || player1.checkSelfCollision();
    bool p2Dead = player2.checkWallCollision() || player2.checkSelfCollision();

    // 互撞检测（头碰对方身体）
    if (!p1Dead) {
        for (size_t i = 0; i < player2.body.size(); ++i)
            if (player1.body[0] == player2.body[i]) p1Dead = true;
    }
    if (!p2Dead) {
        for (size_t i = 0; i < player1.body.size(); ++i)
            if (player2.body[0] == player1.body[i]) p2Dead = true;
    }

    if (p1Dead || p2Dead) {
        gameOver = true;
        if (p1Dead && p2Dead) winner = 3;   // 平局
        else if (p1Dead)      winner = 2;   // 玩家2胜
        else                  winner = 1;   // 玩家1胜
    }
}

void Game::draw() const {
    // 清屏（简单方式：将光标移到 0,0 后重绘）
    gotoxy(0, 0);

    // 绘制上边框
    std::cout << "+" << std::string(WIDTH - 2, '-') << "+" << std::endl;
    for (int y = 1; y < HEIGHT - 1; ++y) {
        std::cout << "|";
        for (int x = 1; x < WIDTH - 1; ++x) {
            Point p(x, y);
            // 检查蛇1
            bool printed = false;
            for (size_t i = 0; i < player1.body.size(); ++i)
                if (player1.body[i] == p) {
                    std::cout << (i == 0 ? 'O' : 'o'); // 头用 O，身体用 o
                    printed = true;
                    break;
                }
            if (!printed) {
                // 检查蛇2
                for (size_t i = 0; i < player2.body.size(); ++i)
                    if (player2.body[i] == p) {
                        std::cout << (i == 0 ? 'X' : 'x');
                        printed = true;
                        break;
                    }
            }
            if (!printed) {
                if (p == food.pos) std::cout << '*';
                else               std::cout << ' ';
            }
        }
        std::cout << "|" << std::endl;
    }
    // 下边框
    std::cout << "+" << std::string(WIDTH - 2, '-') << "+" << std::endl;

    // 分数
    std::cout << "P1(Green/O): " << player1.score << "   P2(Red/X): " << player2.score << std::endl;
    std::cout << "控制: 玩家1 - 方向键 | 玩家2 - WASD" << std::endl;
}

void Game::endScreen() {
    gotoxy(0, HEIGHT + 2);
    if (winner == 1)      std::cout << ">>> 玩家1 (O) 获胜! <<<" << std::endl;
    else if (winner == 2) std::cout << ">>> 玩家2 (X) 获胜! <<<" << std::endl;
    else                  std::cout << ">>> 平局! <<<" << std::endl;
    std::cout << "按 R 重玩 | 按 Q 退出" << std::endl;

    // 等待有效按键
    while (true) {
        if (GetAsyncKeyState('R') & 0x8000) {
            reset();
            break;
        }
        if (GetAsyncKeyState('Q') & 0x8000) {
            system("cls");
            exit(0);
        }
        Sleep(50);
    }
}

void Game::reset() {
    gameOver = false;
    winner = 0;
    player1 = Snake(Point(WIDTH / 2 - 5, HEIGHT / 2), RIGHT);
    player2 = Snake(Point(WIDTH / 2 + 5, HEIGHT / 2), LEFT);
    food.randomPos(player1, player2);
}

void Game::gotoxy(int x, int y) const {
    COORD coord;
    coord.X = (SHORT)x;
    coord.Y = (SHORT)y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void Game::hideCursor() const {
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
}