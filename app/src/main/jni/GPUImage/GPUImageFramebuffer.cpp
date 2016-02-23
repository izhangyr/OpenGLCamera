//
// Created by yongfali on 2016/2/23.
//

#include "GPUImageFrameBuffer.h"

namespace e{

    GPUImageFramebuffer::GPUImageFramebuffer(void)
    {
        _texture = 0;
        _frameBuffer = 0;
    }

    GPUImageFramebuffer::GPUImageFramebuffer(Size size, GPUImageTextureOptions textureOptions, bool onlyGenerateTexture)
    {
        _size = size;
        _textureOptions = textureOptions;
        _frameBufferReferenceCount = 0;
        _ReferenceCountingDisable = false;
        _missingFramebuffer = onlyGenerateTexture;

        if (_missingFramebuffer)
        {

        }
        else
        {

        }
    }

    GPUImageFramebuffer::~GPUImageFramebuffer(void)
    {

    }

}