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
        GPUImageFramebuffer(Size size, GLuint inputTexture);
        GPUImageFramebuffer(Size size, GPUImageTextureOptions textureOptions, bool onlyGenerateTexture);
        virtual ~GPUImageFramebuffer(void);

        void GenerateFramebuffer(void);
        void GenerateTexture(void);
        void DestroyFramebuffer(void);
        //usage
        void ActivateFramebuffer(void);
        //Reference counting
        void Lock(void);
        void Unlock(void);
        void ClearAllLocks(void);
        void DisableReferenceCounting(void);
        void EnableReferenceCounting(void);

        //image capture
        void* NewCGImageFromFramebufferContents;
        void RestoreRenderTarget(void);
        //raw data
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
        bool _referenceCountingDisabled;
#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
        //CVPixelBufferRef _renderTarget;
        //CVOpenGLESTextureRef _renderTexture;
        //int _readLockCount;
#endif
    };

}
#endif
