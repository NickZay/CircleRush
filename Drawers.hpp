#pragma once

std::random_device rd;  // a seed source for the random number engine
std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()
std::uniform_int_distribution<int> heightDist(0, (int)SCREEN_HEIGHT - 1);
std::uniform_int_distribution<int> typeDist(0, 1);

enum SquareType {ADDER, BLOCKER};
const auto DELTA = (double)SQUARE_SIZE / SCREEN_WIDTH;

struct Square {
    int leftY;
    int rightY;
    double part;
    SquareType type;
    Square(int leftY, int rightY, SquareType type) :
        leftY(leftY), rightY(rightY), part(-DELTA), type(type) { }
    Square (std::mt19937& gen) :
        Square(heightDist(gen), heightDist(gen), typeDist(gen) == 0 ? ADDER : BLOCKER) { }
};

bool less(int x, int y, int radius) {
    return sq(x) + sq(y) <= sq(radius);
}


void drawRing(int color) {
    for (int i = -BIG_RADIUS; i <= BIG_RADIUS; ++i) {
        for (int j = -BIG_RADIUS; j <= BIG_RADIUS; ++j) {
            const auto sqrRadius = sq(i) + sq(j);
            if (sq(SMALL_RADIUS) <= sqrRadius && sqrRadius <= sq(BIG_RADIUS)) {
                buffer[i + CENTER_HEIGHT][j + CENTER_WIDTH] = color;
            }
        }
    }
}

void drawCircles(double angle) {
    const auto ballX = getBallX(angle);
    const auto ballY = getBallY(angle);
    for (int i = -BALL_RADIUS; i <= BALL_RADIUS; ++i) {
        for (int j = -BALL_RADIUS; j <= BALL_RADIUS; ++j) {
            if (less(i, j, BALL_RADIUS)) {
                buffer[i + CENTER_HEIGHT + ballY][j + CENTER_WIDTH + ballX] = WHITE;
                buffer[i + CENTER_HEIGHT - ballY][j + CENTER_WIDTH - ballX] = WHITE;
            }
        }
    }
}

void drawSquare(int halfSize, int squareX, int squareY, int color) {
    for (int i = -halfSize; i <= halfSize; ++i) {
        for (int j = -halfSize; j <= halfSize; ++j) {
            const auto x = i + squareX;
            const auto y = j + squareY;
            if ((0 <= x && x < SCREEN_WIDTH) && (0 <= y && y < SCREEN_HEIGHT)) {
                buffer[y][x] = color;
            }
        }
    }
}

void drawSquares(const std::list<Square>& squares) {
    for (const auto& square : squares) {
        const auto squareX = int(square.part * SCREEN_WIDTH);
        const auto squareY = int(square.leftY + square.part * (square.rightY - square.leftY));
        drawSquare(SQUARE_HALF_SIZE, squareX, squareY, square.type == ADDER ? WHITE : PINK);
    }
}

void drawDigit(const int digit, const int rightCornerX, const int rightCornerY) {
    const auto& pixels = pixelsByDigit.at(digit);

    for (int i = 0; i < pixels.size(); ++i) {
        for (int j = 0; j < pixels[i].size(); ++j) {
            if (pixels[i][j] == 1) {
                drawSquare(BLOCK_SIZE / 2,
                           rightCornerX - BLOCK_SIZE * ((int)pixels[i].size() - 1 - j) - (BLOCK_SIZE / 2),
                           rightCornerY + BLOCK_SIZE * i + BLOCK_SIZE / 2, GREEN);
            }
        }
    }
}

void drawNumber(int number, const int rightCornerX, const int rightCornerY) {
    if (number == 0) {
        drawDigit(number % 10, rightCornerX, rightCornerY);
    } else {
        for (int i = 0; number != 0; ++i, number /= 10) {
            drawDigit(number % 10, rightCornerX - (PIXELS_WIDTH + 1) * i * BLOCK_SIZE, rightCornerY);
        }
    }
}

