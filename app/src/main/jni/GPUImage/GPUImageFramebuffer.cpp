//
// Created by yongfali on 2016/2/23.
//

#include "GPUImageFrameBuffer.h"
#include <assert.h>

namespace e{

    inline void SetDefaultTextureOptions(GPUImageOptions& defaultTextureOptions)
    {
        defaultTextureOptions.minFilter = GL_LINEAR;
        defaultTextureOptions.magFilter = GL_LINEAR;
        defaultTextureOptions.wrapS = GL_CLAMP_TO_EDGE;
        defaultTextureOptions.wrapT = GL_CLAMP_TO_EDGE;
        defaultTextureOptions.internalFormat = GL_RGBA;
        defaultTextureOptions.format = GL_BGRA;
        defaultTextureOptions.type = GL_UNSIGNED_BYTE;
    }

    GPUImageFramebuffer::GPUImageFramebuffer(void)
    {
        _texture = 0;
        _frameBuffer = 0;
        _missingFramebuffer = false;
        _frameBufferReferenceCount = 0;
        _referenceCountingDisabled = false;
        SetDefaultTextureOptions(_textureOptions);
    }

    GPUImageFramebuffer::GPUImageFramebuffer(Size size)
    {
        _size = size;
        _texture = 0;
        _frameBuffer = 0;
        _frameBufferReferenceCount = 0;
        _referenceCountingDisabled = false;
        _missingFramebuffer = onlyGenerateTexture;
        SetDefaultTextureOptions(_textureOptions);

        if (_missingFramebuffer)
        {
            // TODO :
            assert(0);
        }
        else
        {
            GenerateFramebuffer();
        }
    }

    GPUImageFramebuffer::GPUImageFramebuffer(Size size, GLuint inputTexture)
    {
        _size = size;
        _frameBuffer = 0;
        _framebufferReferenceCount = 0;
        _referenceCountingDisabled = true;
        _texture = inputTexture;
        SetDefaultTextureOptions(_textureOptions);
    }

    GPUImageFramebuffer::GPUImageFramebuffer(Size size, GPUImageTextureOptions textureOptions, bool onlyGenerateTexture)
    {
        _size = size;
        _texture = 0;
        _frameBuffer = 0;
        _textureOptions = textureOptions;
        _frameBufferReferenceCount = 0;
        _referenceCountingDisable = false;
        _missingFramebuffer = onlyGenerateTexture;

        if (_missingFramebuffer)
        {
            // TODO :
            assert(0);
        }
        else
        {
            GenerateFramebuffer();
        }
    }

    GPUImageFramebuffer::~GPUImageFramebuffer(void)
    {
        DestroyFramebuffer();
    }

    void GPUImageFramebuffer::GenerateTexture(void)
    {
        glActiveTexture(GL_TEXTURE1);
        glGenTextures(1, &_texture);
        glBindTexture(GL_TEXTURE_2D, _texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _textureOptions.minFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _textureOptions.magFilter);
        // This is necessary for non-power-of-two textures
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, _textureOptions.wrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, _textureOptions.wrapT);
    }

    void GPUImageFramebuffer::GenerateFramebuffer(void)
    {
        GPUImageContext::UseImageProcessingContext();

        glGenFramebuffers(1, &_frameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
        if (GPUImageContext::SupportsFastTextureUpload())
        {
#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
            //CVOpenGLESTextureCacheRef coreVideoTextureCache = [[GPUImageContext sharedImageProcessingContext] coreVideoTextureCache];
            //// Code originally sourced from http://allmybrain.com/2011/12/08/rendering-to-a-texture-with-ios-5-texture-cache-api/

            //CFDictionaryRef empty; // empty value for attr value.
            //CFMutableDictionaryRef attrs;
            //empty = CFDictionaryCreate(kCFAllocatorDefault, NULL, NULL, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks); // our empty IOSurface properties dictionary
            //attrs = CFDictionaryCreateMutable(kCFAllocatorDefault, 1, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
            //CFDictionarySetValue(attrs, kCVPixelBufferIOSurfacePropertiesKey, empty);

            //CVReturn err = CVPixelBufferCreate(kCFAllocatorDefault, (int)_size.cx, (int)_size.cy, kCVPixelFormatType_32BGRA, attrs, &renderTarget);
            //if (err)
            //{
            //    NSLog(@"FBO size: %f, %f", _size.width, _size.height);
            //    NSAssert(NO, @"Error at CVPixelBufferCreate %d", err);
            //}

            //err = CVOpenGLESTextureCacheCreateTextureFromImage (kCFAllocatorDefault
            //    , coreVideoTextureCache
            //    , renderTarget
            //    , NULL // texture attributes
            //    , GL_TEXTURE_2D
            //    , _textureOptions.internalFormat// opengl format
            //    , _size.cx
            //    , _size.cy
            //    , _textureOptions.format// native iOS format
            //    , _textureOptions.type
            //    , 0
            //    , &renderTexture);
            //if (err)
            //{
            //    NSAssert(NO, @"Error at CVOpenGLESTextureCacheCreateTextureFromImage %d", err);
            //}

            //CFRelease(attrs);
            //CFRelease(empty);

            //glBindTexture(CVOpenGLESTextureGetTarget(renderTexture), CVOpenGLESTextureGetName(renderTexture));
            //_texture = CVOpenGLESTextureGetName(renderTexture);
            //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, _textureOptions.wrapS);
            //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, _textureOptions.wrapT);

            //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, CVOpenGLESTextureGetName(renderTexture), 0);
#endif
        }
        else
        {
            GenerateTexture();

            glBindTexture(GL_TEXTURE_2D, _texture);
            glTexImage2D(GL_TEXTURE_2D, 0, _textureOptions.internalFormat, (int)_size.width, (int)_size.height, 0, _textureOptions.format, _textureOptions.type, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0);
#ifdef _DEBUG
            GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            assert(status == GL_FRAMEBUFFER_COMPLETE);
#endif
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void GPUImageFramebuffer::DestroyFramebuffer(void)
    {
        GPUImageContext::UseImageProcessingContext();

        if (_frameBuffer)
        {
            glDeleteFramebuffers(1, &_frameBuffer);
            _frameBuffer = 0;
        }

        if (GPUImageContext::SupportsFastTextureUpload() && (!_missingFramebuffer))
        {
#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
            if (_renderTarget)
            {
                CFRelease(_renderTarget);
                _renderTarget = NULL;
            }

            if (_renderTexture)
            {
                CFRelease(_renderTexture);
                _renderTexture = NULL;
            }
#endif
        }
        else
        {
            glDeleteTextures(1, &_texture);
            _texture = 0;
        }
    }

    void GPUImageFramebuffer::ActivateFramebuffer(void)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
        glViewport(0, 0, _size.cx, _size.cy);
    }

    void GPUImageFramebuffer::Lock(void)
    {
        if (_referenceCountingDisable)
        {
            return;
        }

        _frameBufferReferenceCount++;
    }

    void GPUImageFramebuffer::Unlock(void)
    {
        if (_referenceCountingDisable)
        {
            return;
        }

        assert(_frameBufferReferenceCount > 0);
        _frameBufferReferenceCount--;
        if (_frameBufferReferenceCount < 1)
        {
            GPUImageContext::SharedFramebufferCache()
                ->returnFramebufferToCache(this);
        }
    }

    void GPUImageFramebuffer::ClearAllLocks(void)
    {
        _frameBufferReferenceCount = 0;
    }

    void GPUImageFramebuffer::DisableReferenceCounting(void)
    {
        _referenceCountingDisabled = true;
    }

    void GPUImageFramebuffer::EnableReferenceCounting(void)
    {
        _referenceCountingDisabled = false;
    }

    void* GPUImageFramebuffer::NewCGImageFromFramebufferContents(void)
    {
        assert(_textureOptions.internalFormat == GL_RGBA);
        assert(_textureOptions.type == GL_UNSIGNED_BYTE);

        GPUImageContext::useImageProcessingContext();
        int totalBytes = _size.cx * _size.cy * 4;
        GLubyte* rawImagePixels = NULL;
        if (GPUImageContext::SupportsFastTextureUpload())
        {
            //TODO:
            assert(0);
            return NULL;
        }
        else
        {
            ActivateFramebuffer();
            rawImagePixels = (GLubyte*)malloc(totalBytes);
            glReadPixel(0, 0, _size.cx, _size.cy, GL_RGBA, GL_UNSIGNED_BYTE, rawImagePixels);
            Unlock();
            return rawImagePixels;
        }
    }

    void GPUImageFramebuffer::RestoreRenderTarget(void)
    {
#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
        //UnlockAfterReading();
        //CFRelease(_renderTarget);
#endif
    }

    void GPUImageFramebuffer::lockForReading(void)
    {
#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
        if (GPUImageContext::SupportsFastTextureUpload())
        {
            if (_readLockCount == 0)
            {
                //CVPixelBufferLockBaseAddress(renderTarget, 0);
            }
            _readLockCount++;
        }
#endif
    }

    void GPUImageFramebuffer::unlockAfterReading(void)
    {
#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
        if (GPUImageContext::SupportsFastTextureUpload())
        {
            assert(_readLockCount > 0);
            _readLockCount--;
            if (_readLockCount == 0)
            {
                CVPixelBufferUnlockBaseAddress(_renderTarget, 0);
            }
        }
#endif
    }

    int GPUImageFramebuffer::BytesPerRow(void)
    {
        if (GPUImageContext::SupportsFastTextureUpload())
        {
#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
            //return CVPixelBufferGetBytesPerRow(renderTarget);
#else
            return _size.cx * 4; // TODO: do more with this on the non-texture-cache side
#endif
        }
        else
        {
            return _size.cx * 4;
        }
    }

    GLubyte* GPUImageFramebuffer::ByteBuffer(void)
    {
#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
        lockForReading();
        //GLubyte* bufferBytes = CVPixelBufferGetBaseAddress(_renderTarget);
        unlockAfterReading();
        return bufferBytes;
#else
        return NULL; // TODO: do more with this on the non-texture-cache side
#endif
    }

    GLuint GPUImageFramebuffer::Texture(void)
    {
        return _texture;
    }
}