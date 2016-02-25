//
// Created by yongfali on 2016/2/25.
//

#include "GLFramebufferCache.h"
#include <functional>
#include <string>

namespace e {

GLFramebufferCache::GLFramebufferCache(void)
{

}

GLFramebufferCache::~GLFramebufferCache(void)
{
    std::map<size_t, GLFramebuffer*>::iterator it = _frameBufferCache.begin();
    for (;it != _frameBufferCache.end(); it++)
    {
        if (it->second)
        {
            delete (it->second);
        }
    }
}

GLFramebuffer* GLFramebufferCache::FetchFramebuffer(Size size, bool onlyTexture)
{
    GLTextureOptions defaultTextureOptions;
    defaultTextureOptions.minFilter = GL_LINEAR;
    defaultTextureOptions.magFilter = GL_LINEAR;
    defaultTextureOptions.wrapS = GL_CLAMP_TO_EDGE;
    defaultTextureOptions.wrapT = GL_CLAMP_TO_EDGE;
    defaultTextureOptions.internalFormat = GL_RGBA;
    defaultTextureOptions.format = GL_BGRA;
    defaultTextureOptions.type = GL_UNSIGNED_BYTE;

    return FetchFramebuffer(size, defaultTextureOptions, onlyTexture);
}

GLFramebuffer* GLFramebufferCache::FetchFramebuffer(Size size, GLTextureOptions textureOptions, bool onlyTexture)
{
    GLFramebuffer* frameBuffer = NULL;
    size_t key = Hash(size, textureOptions, onlyTexture);
    std::map<size_t,GLFramebuffer*>::iterator it = _frameBufferCache.find(key);
    if (it == _frameBufferCache.end())
    {
        frameBuffer = new GLFramebuffer(size, textureOptions, onlyTexture);
        assert(frameBuffer);
    }
    else
    {
        frameBuffer = it->second;
        _frameBufferCache.erase(it);
    }

    return frameBuffer;
}

void GLFramebufferCache::ReturnFramebuffer(GLFramebuffer* frameBuffer)
{
    //frameBuffer->ClearAllLocks();
    Size size = frameBuffer->Size();
    GLTextureOptions textureOptions = frameBuffer->TextureOptions();
    size_t key = Hash(size, textureOptions, frameBuffer->MissingFrameBuffer());
    _frameBufferCache.insert(pair<size_t, GLFramebuffer*>(key, frameBuffer));
}

size_t GLFramebufferCache::Hash(Size size, GLTextureOptions& textureOptions, bool onlyTexture)
{
    std::string key(1024);
    if (onlyTexture)
    {
        sprintf(key.c_str(), "%dx%d-%d:%d:%d:%d:%d:%d:%d-NOFB"
            , size.cx
            , size.cy
            , textureOptions.minFilter
            , textureOptions.magFilter
            , textureOptions.wrapS
            , textureOptions.wrapT
            , textureOptions.internalFormat
            , textureOptions.format
            , textureOptions.type);
    }
    else
    {
        sprintf(key.c_str(), "%dx%d-%d:%d:%d:%d:%d:%d:%d"
            , size.cx
            , size.cy
            , textureOptions.minFilter
            , textureOptions.magFilter
            , textureOptions.wrapS
            , textureOptions.wrapT
            , textureOptions.internalFormat
            , textureOptions.format
            , textureOptions.type);
    }

    std::hash(string) h;
    return h(key);
}

}