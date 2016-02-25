//
// Created by yongfali on 2016/2/24.
//

#ifndef _E_GLCONTEXT_H_
#define _E_GLCONTEXT_H_

#include <EGL/egl.h>
#include <EGL/gl.h>
#include "GLBase.h"
#include <map>

namespace e{

#define GPUImageRotationSwapsWidthAndHeight(rotation) \
    ((rotation) == kGPUImageRotateLeft || (rotation) == kGPUImageRotateRight \
    || (rotation) == kGPUImageRotateRightFlipVertical || (rotation) == kGPUImageRotateRightFlipHorizontal)

    typedef enum {
        kGLNoRotation,
        kGLRotateLeft,
        kGLRotateRight,
        kGLFlipVertical,
        kGLFlipHorizontal,
        kGLRotateRightFlipVertical,
        kGLRotateRightFlipHorizontal,
        kGLRotate180
     } GLRotationMode;

    class GLContext {
    public:
        GLContext(void);
        virtual ~GLContext(void);

        bool Initialize(void);
        void Release(void);

        static GLContext* SharedImageProcessingContext(void);
        static void UseImageProcessingContext(void);
        static void SetActiveShaderProgram(GLProgram* shaderProgram);
        static bool SupportsFastTextureUpload(void);

        void UserAsCurrentContext(void);
        void SetContextShaderProgram(GLProgram*shaderProgram);
        GLProgram* ProgramFromCache(const char* vertexShaderString, const char* fragmentShaderString);

    private:
        EGLDisplay* _eglDisplay;
        EGLSurface* _eglSurface;
        EGLContext* _eglContext;

        GLProgram* _currentShaderProgram;
        std::map<size_t, GLProgram*> _shaderProgramCache;
    };

    class GLInput
    {
    public:
        virtual void NewFrameReady(time_t frameTime, int textureIndex) = 0;
        virtual void SetInputFramebuffer(GLFrameBuffer* inputFramebuffer, int textureIndex) = 0;
        virtual int NextAvailableTextureIndex(void) = 0;
        virtual void SetInputSize(Size size, int textureIndex) = 0;
        virtual void SetInputRotation(GLRotationMode inputRotation, int textureIndex) = 0;
        virtual Size MaximumOutputSize(void) = 0;
        virtual void EndProcessing(void) = 0;
        virtual bool ShouldIgnoreUpdatesToThisTarget(void) = 0;
        virtual bool Enable(void) = 0;
        virtual bool WantsMonochromeInput(void) = 0;
        virtual void SetCurrentlyReceivingMonochromeInput(bool value) = 0;
    };

}

#endif //OPENGLCAMERA_GLCONTEXT_H
