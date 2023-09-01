#include "../include/Core/Texture.hpp"

Texture::Texture()
{

}

void Texture::LoadTexture(std::string location)
{
    SDL_Surface *image;
    GLenum textureFormat;
    GLenum pixelSize;
    int bpp;
    image = IMG_Load(location.c_str());
    if(image == nullptr)
        DebugLog("Could not find image file " + location);

    bpp = image->format->BytesPerPixel;
    if (bpp == 4)
    {
        pixelSize = GL_RGBA;
        if (image->format->Rmask == 0x000000ff)
            textureFormat = GL_RGBA;
        else
            textureFormat = GL_BGRA;
    }
    else if (bpp == 3)
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