//
// Created by yongfali on 2016/2/25.
//

#ifndef _E_GLFRAMEBUFFERCACHE_H_
#define _E_GLFRAMEBUFFERCACHE_H_
#include <map>

namespace e{

class GLFramebufferCache {
public:
    GLFramebufferCache(void);
    virtual ~GLFramebufferCache(void);
    GLFramebuffer* FetchFramebuffer(Size size, bool onlyTexture);
    GLFramebuffer* FetchFramebuffer(Size size, GLTextureOptions textureOptions, bool onlyTexture);
    void ReturnFramebuffer(GLFramebuffer* frameBuffer);
private:
    size_t Hash(Size size, GLTextureOptions& textureOptions, bool onlyTexture);
protected:
    std::map<size_t, GLframebuffer*> _frameBufferCache;
};

}

#endif
