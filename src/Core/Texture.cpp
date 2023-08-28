#include "../include/Core/Texture.hpp"

Texture::Texture(std::string location)
{
    SDL_Surface *image;
    GLenum textureFormat;
    int bpp;
    image = IMG_Load(location.c_str());
    if(image == nullptr)
        DebugLog("Could not find image file " + location);

    bpp = image->format->BytesPerPixel;                                      
    if (bpp == 4) 
    {                                                          
        if (image->format->Rmask == 0x000000ff)                          
            textureFormat = GL_RGBA;                                 
        else                                                             
            textureFormat = GL_BGRA;                                 
    } 
    else if (bpp == 3) 
    {                                                   
        if (image->format->Rmask == 0x000000ff)                          
            textureFormat = GL_RGB;                                  
        else                                                             
            textureFormat = GL_BGR;                                  
    }
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, bpp, image->w, image->h, 0, textureFormat, GL_UNSIGNED_BYTE, image->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glBindTexture(GL_TEXTURE_2D, 0);
    SDL_FreeSurface(image);
}