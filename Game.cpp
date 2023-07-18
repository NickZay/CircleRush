#include "Engine.h"
#include <stdlib.h>
#include <memory.h>
#include <vector>
#include <list>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <random>

#include "Colors.hpp"
#include "Utils.hpp"
#include "Drawers.hpp"


//  is_key_pressed(int button_vk_code) - check if a key is pressed,
//                                       use keycodes (VK_SPACE, VK_RIGHT, VK_LEFT, VK_UP, VK_DOWN, VK_RETURN)
//  get_cursor_x(), get_cursor_y() - get mouse cursor position
//  is_mouse_button_pressed(int button) - check if mouse button is pressed (0 - left button, 1 - right button)
//  schedule_quit_game() - quit game after act()


enum Direction {CLOCKWISE, COUNTERCLOCKWISE};

static int maxPoints = 0;

struct Game {
    bool active;
    int points;
    Direction direction;
    double angle;
    bool spaceWasPressed = false;
    std::list<Square> squares;
    Game() : active(false), points(0), direction(CLOCKWISE), angle(0) { }
};

static Game g;

// initialize game data in this function
void initialize() { }

void handleSpace() {
    bool spacePressed = is_key_pressed(VK_SPACE);
    if (!g.spaceWasPressed && spacePressed) {
        if (g.active) {
            g.direction = (g.direction == CLOCKWISE ? COUNTERCLOCKWISE : CLOCKWISE);
        } else {
            g.active = true;
        }
        g.spaceWasPressed = spacePressed;
    } else if (g.spaceWasPressed && !spacePressed) {
        g.spaceWasPressed = spacePressed;
    }
}

void removeAndAddSquares() {
    while (g.squares.size() > 0 && g.squares.back().part >= 1 + DELTA) {
        g.squares.pop_back();
    }
    if (g.squares.size() < 1) {
        g.squares.push_back(Square(gen));
    } else if (g.squares.size() < 2) {
        if (g.squares.front().part > 0.5) {
            g.squares.push_front(Square(gen));
        }
    }
}

bool handleIntersection(const Square& square) {
    if (square.type == BLOCKER) {
        g = {};
        return true;
    }
    g.points += 1;
    maxPoints = std::max(maxPoints, g.points);
    return false;
}

void checkSquares(double dt) {
    const auto ballX = getBallX(g.angle);
    const auto ballY = getBallY(g.angle);
    
    for (auto it = std::begin(g.squares); it != std::end(g.squares); ) {
        auto& square = *it;
        square.part += dt / SLOW_COEF;
        const auto squareX = int(square.part * SCREEN_WIDTH);
        const auto squareY = int(square.leftY + square.part * (square.rightY - square.leftY));
        if (less(squareX - (CENTER_WIDTH + ballX), squareY - (CENTER_HEIGHT + ballY), SQUARE_SIZE) ||
            less(squareX - (CENTER_WIDTH - ballX), squareY - (CENTER_HEIGHT - ballY), SQUARE_SIZE)) {
            if (handleIntersection(square)) break;
            g.squares.erase(it++);
        } else {
            ++it;
        }
    }
}


// this function is called to update game data,
// dt - time elapsed since the previous update (in seconds)
void act(float dt) {
    if (is_key_pressed(VK_ESCAPE)) {
        schedule_quit_game();
    }
    handleSpace();
    if (!g.active) {
        return;
    }
    g.angle += (g.direction == CLOCKWISE ? -1 : 1) * SPEED_COEF * dt;
    removeAndAddSquares();
    checkSquares(dt);
}


// fill buffer in this function
// uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH] - is an array of 32-bit colors (8 bits per R, G, B)
void draw() {
    // clear backbuffer
    memset(buffer, BLACK, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(uint32_t));
    
    drawRing(g.direction == CLOCKWISE ? YELLOW : AZURE);
    drawCircles(g.angle);
    drawSquares(g.squares);
    drawNumber(g.points, SCREEN_WIDTH - BLOCK_SIZE, BLOCK_SIZE);
    drawNumber(maxPoints, SCREEN_WIDTH - BLOCK_SIZE, (1 + PIXELS_HEIGHT + 1) * BLOCK_SIZE);
}

// free game data in this function
void finalize() { }
