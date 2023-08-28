#pragma once
#include "../glad/glad.h"
#include "Debug.hpp"
#include <SDL2/SDL_image.h>
#include <string>

class Texture
{
public:
    unsigned int id;
    std::string textLocation;
    Texture();
    void LoadTexture(std::string location);
};
