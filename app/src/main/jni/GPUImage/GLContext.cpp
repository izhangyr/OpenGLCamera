//
// Created by yongfali on 2016/2/24.
//

#include "GLContext.h"
#include <functional>
#include <string>

namespace e{

    GLContext::GLContext(void)
    {
        _eglContext = EGL_NO_CONTEXT;
        _eglSurface = EGL_NO_SURFACE;
        _eglDisplay = EGL_NO_DISPLAY;
        _currentShaderProgram = 0;
        _frameBufferCache = new GLFramebufferCache();
        assert(_frameBufferCache);
    }

    GLContext::~GLContext(void)
    {
        std::map<size_t, GLProgram*>::iterator it = _shaderProgramCache.begin();
        for(; it!=_shaderProgramCache.end(); it++)
        {
            if (it->second)
                delete (it->second);
        }
        if (_frameBufferCache)
        {
            delete _frameBufferCache;
            _frameBufferCache;
        }

        if (_eglContext != EGL_NO_CONTEXT)
        {
            ReleaseContext();
        }
    }

    bool GLContext::CreateContext(void)
    {
        LOGD("EGL Step 1: Get default display.");
        if ((_eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY) == EGL_NO_DISPLAY){
            LOGE("Error : eglGetDisplay() return error %d\n", eglGetError());
            return false;
        }

        LOGD("EGL Step 2: Initialize EGL");
        if (_eglContext == 0){
            if (!eglInitialize(_eglDisplay, 0, 0)){
                LOGE("Error : eglInitialize() return error %d\n", eglGetError());
                return false;
            }

            LOGD("EGL Step 3: Make OpenGL ES the current API.");
            eglBindAPI(EGL_OPENGL_ES_API);

            LOGD("EGL Step 4: Specify the required configuration attributes.");
            EGLint pi32ConfigAttributes[] = {
                //EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
                EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                EGL_RED_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_BLUE_SIZE, 8,
                EGL_ALPHA_SIZE, 8,
                EGL_DEPTH_SIZE, 0,
                //EGL_STENCIL_SIZE, 0,
                EGL_NONE
            };

            int iConfigs;
            EGLConfig eglConfig;
            eglChooseConfig(_eglDisplay, pi32ConfigAttributes, &eglConfig, 1, &iConfigs);
            if (iConfigs != 1) {
                LOGE("Error: eglChooseConfig(): config not found %d - %d.\n", eglGetError(), iConfigs);
                return false;
            }

            LOGD("EGL Step 6: Create a surface to draw to.");
            EGLint PBufAttributes[] = {
            //EGL_WIDTH, 32, EGL_HEIGHT, 48,
            //EGL_LARGEST_PBUFFER, EGL_TRUE,
                EGL_NONE
            };
            _eglSurface = eglCreatePbufferSurface(eglDisplay, eglConfig, PBufAttributes);
            if (_eglSurface == EGL_NO_SURFACE){
                LOGE("Error : eglCreatePbufferSurface() return error %d\n", eglGetError());
                return false;
            }

            LOGD("EGL Step 7: Create a context.");
            const EGLint attributes[] = {
                EGL_CONTEXT_CLIENT_VERSION,
                2,
                EGL_NONE
            };
            _eglContext = eglCreateContext(eglDisplay, eglConfig, EGL_NO_CONTEXT, attributes);
            if (_eglContext = EGL_NO_CONTEXT){
                LGOE("Error : eglCreateContext() return error %d\n", eglGetError());
                return false;
            }
        }

        // Step 8 - Bind the context to the current thread.
        if (!eglMakeCurrent(_eglDisplay, _eglSurface, _eglSurface, _eglContext)){
            LOGE("Error : eglMakeCurrent() failed\n");
            return false;
        }
        return true;
    }

    void GLContext::ReleaseContext(void)
    {
        eglMakeCurrent(_eglDisplay, EGL_NO_SURFACE , EGL_NO_SURFACE , EGL_NO_CONTEXT);
        eglDestroySurface(_eglDisplay, _eglSurface);
        eglDestroyContext(_eglDisplay, _eglContext);
        eglTerminate(_eglDisplay);
    }


    GLContext* GLContext::SharedImageProcessingContext(void)
    {
        static GLContext* sharedImageProcessingContext = NULL;
        if (sharedImageProcessingContext == NULL){
            sharedImageProcessingContext = new GLContext();
            if (sharedImageProcessingContext == NULL){
                LOGE("Error : GLContext new failed");
            }
        }
        return sharedImageProcessingContext;
    }

    void GLContext::UseImageProcessingContext(void)
    {
        SharedImageProcessingContext()->UseAsCurrentContext();
    }

    void GLContext::UseAsCurrentContext(void)
    {
        if (_eglContext == EGL_NO_CONTEXT)
        {
             bool ret = CreateContext();
             assert(ret);
        }
        assert(_eglContext != EGL_NO_CONTEXT);
    }

    void GLContext::SetActiveShaderProgram(GLProgram* shaderProgram)
    {
        assert(shaderProgram);
        if (_currentShaderProgram != shaderProgram){
            _currentShaderProgram = shaderProgram;
            shaderProgram->Use();
        }
    }

    void GLContext::SetContextShaderProgram(GLProgram* shaderProgram)
    {
        SetActiveShaderProgram(shaderProgram);
    }

    GLProgram* GLContext::GetProgram(const char* vertexShaderString, const char* fragmentShaderString)
    {
        std::hash<string> h;
        std::string text = vertexShaderString;
        text.append(fragmentShaderString);
        size_t key = h(text);

        GLProgram* program = NULL;
        std::map<size_t, GLProgram*>::iterator it = _shaderProgramCache.fine(key);
        if (it == _shaderProgramCache.end())
        {
            program = new GLProgram(vertexShaderString, fragmentShaderString);
            assert(program);
            _shaderProgramCache->insert(pair<size_t, GLProgram*>(key, program));
        }
        else
        {
            program = it->second;
        }

        return program;
    }

    GLFramebufferCache* GLContext::SharedFramebufferCache(void)
    {
        return _frameBufferCache;
    }

}
