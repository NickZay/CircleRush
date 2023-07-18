#pragma once

#include <unordered_map>
#include "Engine.h"
#include <cmath>

const auto CENTER_HEIGHT = SCREEN_HEIGHT / 2;
const auto CENTER_WIDTH = SCREEN_WIDTH / 2;
const auto MAIN_RADIUS = std::min(SCREEN_HEIGHT, SCREEN_WIDTH) / 4;
const auto DX = 4;
const auto BLOCK_SIZE = DX * 2;
const auto BIG_RADIUS = MAIN_RADIUS + DX;
const auto SMALL_RADIUS = MAIN_RADIUS - DX;
const auto BALL_RADIUS = std::max(2 * DX, MAIN_RADIUS / 8);
const auto SQUARE_SIZE = BALL_RADIUS * 2;
const auto SQUARE_HALF_SIZE = SQUARE_SIZE / 2;


const auto SPEED_COEF = 3;
const auto SLOW_COEF = 2;

template <typename T>
T sq(const T& a) {
    return a * a;
}

int getBallX(double angle) {
    return cos(angle) * MAIN_RADIUS;
}
int getBallY(double angle) {
    return sin(angle) * MAIN_RADIUS;
}

const auto PIXELS_WIDTH = 3;
const auto PIXELS_HEIGHT = 5;

const std::unordered_map<int, std::vector<std::vector<int>>> pixelsByDigit = {
    {
        0,
        {
            {1, 1, 1},
            {1, 0, 1},
            {1, 0, 1},
            {1, 0, 1},
            {1, 1, 1}
        },
    },
    {
        1,
        {
            {0, 1, 0},
            {1, 1, 0},
            {0, 1, 0},
            {0, 1, 0},
            {1, 1, 1},
        },
    },
    {
        2,
        {
            {1, 1, 1},
            {0, 0, 1},
            {1, 1, 1},
            {1, 0, 0},
            {1, 1, 1},
        },
    },
    {
        3,
        {
            {1, 1, 1},
            {0, 0, 1},
            {1, 1, 1},
            {0, 0, 1},
            {1, 1, 1},
        },
    },
    {
        4,
        {
            {1, 0, 1},
            {1, 0, 1},
            {1, 1, 1},
            {0, 0, 1},
            {0, 0, 1},
        },
    },
    {
        5,
        {
            {1, 1, 1},
            {1, 0, 0},
            {1, 1, 1},
            {0, 0, 1},
            {1, 1, 1},
        }
    },
    {
        6,
        {
            {1, 1, 1},
            {1, 0, 0},
            {1, 1, 1},
            {1, 0, 1},
            {1, 1, 1},
        },
    },
    {
        7,
        {
            {1, 1, 1},
            {0, 0, 1},
            {0, 1, 0},
            {0, 1, 0},
            {0, 1, 0},
        },
    },
    {
        8,
        {
            {1, 1, 1},
            {1, 0, 1},
            {1, 1, 1},
            {1, 0, 1},
            {1, 1, 1},
        },
    },
    {
        9,
        {
            {1, 1, 1},
            {1, 0, 1},
            {1, 1, 1},
            {0, 0, 1},
            {1, 1, 1},
        }
    }
};
