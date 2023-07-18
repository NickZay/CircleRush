#pragma once

//
//  DO NOT MODIFY THIS FILE
//

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

// backbuffer
extern uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH];

enum
{
  VK_ESCAPE,
  VK_SPACE,
  VK_LEFT,
  VK_UP,
  VK_RIGHT,
  VK_DOWN,
  VK_RETURN,

  VK__COUNT
};

// VK_SPACE, VK_RIGHT, VK_LEFT, VK_UP, VK_DOWN, etc.
bool is_key_pressed(int button_vk_code);
bool is_mouse_button_pressed(int mouse_button);
int get_cursor_x(void);
int get_cursor_y(void);

void initialize(void);
void finalize(void);

void act(float dt);
void draw(void);

void schedule_quit_game(void);


#ifdef __cplusplus
}
#endif
