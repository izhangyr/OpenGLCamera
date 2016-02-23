//
// Created by yongfali on 2016/2/23.
//

#ifndef _E_GPUIMAGEFRAMEBUFFER_H_
#define _E_GPUIMAGEFRAMEBUFFER_H_
#include <OpenGLES/EAGL.h>
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#include "BaseClasses.h"

namespace e{

    typedef struct GPUTextureOptions {
        GLenum minFilter;
        GLenum magFilter;
        GLenum wrapS;
        GLenum wrapT;
        GLenum internalFormat;
        GLenum format;
        GLenum type;
    } GPUTextureOptions;

    class GPUImageFramebuffer
    {
    public:
        GPUImageFramebuffer(void);
        GPUImageFramebuffer(Size size);
        GPUImageFramebuffer(Size size, GPUImageTextureOptions textureOptions, bool onlyGenerateTexture);
        GPUImageFramebuffer(Size size, GLuint inputTexture);
        virtual ~GPUImageFramebuffer(void);

        void GenerateFramebuffer(void);
        void GenerateTexture(void);
        void DestroyFramebuffer(void);

        void ActivateFramebuffer(void);
        void Lock(void);
        void Unlock(void);
        void ClearAllLocks(void);
        void DisableReferenceCounting(void);
        void EnableReferenceCounting(void);
        void LockForReading(void)
        void UnlockAfterReading(void);

        int BytesPerRow(void);
        GLubyte* ByteBuffer(void);
    private:
        Size _size;
        GLuint _texture;
        GLuint _frameBuffer;
        GPUTextureOptions _textureOptions;
        bool _missingFramebuffer;
        int _frameBufferReferenceCount;
        bool _referenceCountingDisable;
    };

}
#endif
