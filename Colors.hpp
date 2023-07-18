#ifndef Colors_hpp
#define Colors_hpp

uint32_t Color(uint8_t r, uint8_t g, uint8_t b, uint8_t x) {
    uint32_t c;
    c = r;
    c <<= 8;
    c |= g;
    c <<= 8;
    c |= b;
    c <<= 8;
    c |= x;
    return c;
}

const auto BLACK = Color(0, 0, 0, 0);
const auto RED = Color(0, 255, 0, 0);
const auto GREEN = Color(0, 0, 255, 0);
const auto BLUE = Color(0, 0, 0, 255);
const auto YELLOW = Color(0, 255, 255, 0);
const auto PINK = Color(0, 255, 0, 255);
const auto AZURE = Color(0, 0, 255, 255);
const auto WHITE = Color(0, 255, 255, 255);

#endif /* Colors_hpp */
