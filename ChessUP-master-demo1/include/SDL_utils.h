#ifndef SDL_UTILS__H
#define SDL_UTILS__H

#include <iostream>
#include <SDL.h>

//#define LAB
//#define BOARD

//*****************************************************
// Các hàm chung về khởi tạo và huỷ SDL

void logSDLError(std::ostream& os, const std::string &msg, int error_code, bool fatal = false);

void initSDL(SDL_Window* &window, SDL_Renderer* &renderer, int screenWidth, int screenHeight, const char* windowTitle) ;

void quitSDL(SDL_Window* window, SDL_Renderer* renderer);

void waitUntilKeyPressed();

void updateScreen(SDL_Window* window, SDL_Renderer* renderer);

#endif
