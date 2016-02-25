//
// Created by yongfali on 2016/2/25.
//

#include "GLTwoPassFilter.h"

namespace e {

    GLTwoPassFilter::GLTwoPassFilter(void)
    {
        _secondOutputFramebuffer = 0;
        _secondFilterProgram = 0;
        _secondFilterPositionAttribute = 0;
        _secondFilterTextureCoordinateAttribute = 0;
        _secondFilterInputTextureUniform = 0;
        _secondFilterInputTextureUniform2 = 0;
    }

    GLTwoPassFilter::~GLTwoPassFilter(void)
    {

    }

    bool GLTwoPassFilter::Initialize(const char* firstStageVertexShaderString
        , const char* firstStageFragmentShaderString
        , const char* secondStageVertexShaderString
        , const char* secondStageFragmentShaderString)
    {
        bool ret = GLShaderFilter::Initialize(firstStateVertexShaderString, firstStageFragmentShaderString);
        if (!ret) return false;

        GLContext* context = GLContext::UseImageProcessingContext();
        _secondFilterProgram = context->Program(secondStageVertexShaderString, secondStageFragmentShaderString);
        if (!_secondFilterProgram->Initialized())
        {
            InitializeSecondaryAttributes();

            if (!_secondFilterProgram->Link())
            {
                std::string programLog = _secondFilterProgram->ProgramLog();
                LOGE("Error:GLSL link error program log -> %s", programLog.c_str());
                std::string vertexShaderLog = _secondFilterProgram->VertexShaderLog();
                LOGE("Error:GLSL link error vertex shader log -> %s", vertexShaderLog.c_str());
                std::string fragmentShaderLog = _secondFilterProgram->FragmentShaderLog();
                LOGE("Error:GLSL link error fragment shader log -> %s", fragmentShaderLog.c_str());
                assert(false);
                return false;
            }
        }

        _secondFilterPositionAttribute = _secondFilterProgram->AttributeIndex("position");
        _secondFilterTextureCoordinateAttribute = _secondFilterProgram->AttributeIndex("inputTextureCoordinate");
        _secondFilterInputTextureUniform = _secondFilterProgram->AttributeIndex("inputImageTexture");
        _secondFilterInputTextureUniform2 = _secondFilterProgram->AttributeIndex("inputImageTexture2");

        GLContext::SetActiveShaderProgram(_secondFilterProgram);
        glEnableVertexAttribArray(secondFilterPositionAttribute);
        glEnableVertexAttribArray(secondFilterTextureCoordinateAttribute);
        return true;
    }

    bool GLTwoPassFilter::Initialize(const char* firstStageFragmentShaderString,  const char* secondStageFragmentShaderString)
    {
        return Initialize(kGLVertexShaderString
            , firstStageFragmentShaderString
            , kGLVertexShaderString
            , secondStageFragmentShaderString);
    }

    void GLTwoPassFilter::InitializeSecondaryAttributes(void)
    {
        _secondFilterProgram->AddAttribute("position");
        _secondFilterProgram->AddAttribute("inputTextureCoordinate");
    }

    GLFramebuffer* GLTowPassFilter::FramebufferForOutput(void)
    {
        return _secondFramebuffer;
    }

    void GLTowPassFilter::RemoveOutputFramebuffer(void)
    {
        _secondFramebuffer = NULL;
    }

    void RenderToTexture(const GLfloat* vertices, const GLfloat* textureCoordinates)
    {
        if (_preventRendering)
        {
            _firstInputFramebuffer->Unlock();
            return;
        }

        GLContext::SetActiveShaderProgram(_filterProgram);

        _outputFramebuffer = GLContext::SharedFramebufferCache()->FetchFramebuffer(SizeOfFBO(), _outputTextureOptions, false);
        _outputFramebuffer->ActiveFramebuffer();

        SetUniforms(0);

        glClearColor(backgroundColorRed, backgroundColorGreen, backgroundColorBlue, backgroundColorAlpha);
        glClear(GL_COLOR_BUFFER_BIT);

    	glActiveTexture(GL_TEXTURE2);
    	glBindTexture(GL_TEXTURE_2D, _firstInputFramebuffer->Texture());

    	glUniform1i(_filterInputTextureUniform, 2);

        glVertexAttribPointer(_filterPositionAttribute, 2, GL_FLOAT, 0, 0, vertices);
    	glVertexAttribPointer(_filterTextureCoordinateAttribute, 2, GL_FLOAT, 0, 0, textureCoordinates);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        firstInputFramebuffer->Unlock();
        firstInputFramebuffer = NULL;

        // Run the second stage of the two-pass filter
        _secondOutputFramebuffer = GLContext::SharedFramebufferCache()->FetchFramebuffer(SizeOfFBO(), _outputTextureOptions, false);
        _secondOutputFramebuffer->ActiveFramebuffer();
        GLContext::SetActiveShaderProgram(_secondOutputFramebuffer);
        if (_usingNextFrameForImageCapture)
        {
            _secondOutputFramebuffer->Lock();
        }

        SetUniforms(1);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, _outputFramebuffer->Texture());
        glVertexAttribPointer(_secondFilterTextureCoordinateAttribute, 2, GL_FLOAT, 0, 0, TextureCoordinates(kGPUImageNoRotation]));

    	glUniform1i(_secondFilterInputTextureUniform, 3);

        glVertexAttribPointer(_secondFilterPositionAttribute, 2, GL_FLOAT, 0, 0, vertices);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        _outputFramebuffer->Unlock();
        _outputFramebuffer = NULL;
    }

    void GLTwoPassFilter::SetUniforms(int programIndex)
    {

    }

}