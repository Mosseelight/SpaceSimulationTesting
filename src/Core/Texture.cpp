#include "../include/Core/Texture.hpp"

Texture::Texture()
{

}

void Texture::LoadTexture(std::string location)
{
    SDL_Surface *image = nullptr;
    GLenum textureFormat;
    GLenum pixelSize;
    int bpp = 3;
    unsigned char pixels[64][64][3];
    image = IMG_Load(location.c_str());
    if(image == nullptr)
    {
        DebugLog("Could not find image file " + location);
        image = SDL_CreateRGBSurface(0, 64, 64, 8, 0, 0, 0, 0);
        pixelSize = GL_RGB;
        textureFormat = GL_RGB;
        for (unsigned int x = 0; x < 64; x++)
        {
            for (unsigned int y = 0; y < 64; y++)
            {
                if ((x / 4 + y / 4) % 2 == 0)
                {
                    pixels[x][y][0] = 255;
                    pixels[x][y][1] = 0;
                    pixels[x][y][2] = 255;
                }
                else
                {
                    pixels[x][y][0] = 0;
                    pixels[x][y][1] = 0;
                    pixels[x][y][2] = 0;
                }
            }
        }
    }

    bpp = image->format->BytesPerPixel;
    if (bpp == 4)
    {
        pixelSize = GL_RGBA;
        if (image->format->Rmask == 0x000000ff)
            textureFormat = GL_RGBA;
        else
            textureFormat = GL_BGRA;
    }
    if (bpp == 3)
    {
        pixelSize = GL_RGB;
        if (image->format->Rmask == 0x000000ff)
            textureFormat = GL_RGB;
        else
            textureFormat = GL_BGR;
    }
    glGenTextures(1, &id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    if(image == nullptr)
        glTexImage2D(GL_TEXTURE_2D, 0, pixelSize, image->w, image->h, 0, textureFormat, GL_UNSIGNED_BYTE, pixels);
    else
        glTexImage2D(GL_TEXTURE_2D, 0, pixelSize, image->w, image->h, 0, textureFormat, GL_UNSIGNED_BYTE, image->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    SDL_FreeSurface(image);
    textLocation = location;
}

void Texture::Delete()
{
    glDeleteTextures(1, &id);
}