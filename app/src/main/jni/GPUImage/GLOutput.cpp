//
// Created by yongfali on 2016/2/25.
//

#include "GLOutput.h"

namespace e{

    GLOutput::GLOutput(void)
    {
        _outputFramebuffer = 0;
        _overrideInputSize = false;;
        _allTargetsWantMonochromeData = false;
        _usingNextFrameForImageCapture = false;

        _shouldSmoothlyScaleOutput = false;
        _shouldIgnoreUpdatesToThisTarget = false;
        _enable = false;
        _targetToIgnoreForUpdates = NULL;

        _outputTextureOptions.minFilter = GL_LINEAR;
        _outputTextureOptions.magFilter = GL_LINEAR;
        _outputTextureOptions.wrapS = GL_CLAMP_TO_EDGE;
        _outputTextureOptions.wrapT = GL_CLAMP_TO_EDGE;
        _outputTextureOptions.internalFormat = GL_RGBA;
        _outputTextureOptions.format = GL_BGRA;
        _outputTextureOptions.type = GL_UNSIGNED_BYTE;
    }

    GLOutput::~GLOutput(void)
    {

    }

    void GLOutput::SetInputFramebuffer(GLInput* target, int inputTextureIndex)
    {
        target->SetInputFramebuffer(FramebufferForOutput(), inputTextureIndex);
    }

    GLFramebuffer* GLOutput::FramebufferForOutput(void)
    {
        return _outputFramebuffer;
    }

    void GLOutput::RemoveOutputFramebuffer(void)
    {
        _outputFramebuffer = NULL;
    }

    void GLOutput::NotifyTargetsAboutNewOutputTexture(void)
    {
        for (size_t i=0; i<_targets.size(); i++)
        {

        }
    }

    std::vector<GLInput*> GLOutput::Targets(void)
    {
        return _targets;
    }

    void GLOutput::AddTarget(GLInput* target)
    {
        int nextAvailableTextureIndex = target->NextAvailableTextureIndex();
        AddTarget(target, nextAvailableTextureIndex);

        if (target->ShouldIgnoreUpdatesToThisTarget)
        {
            _targetToIgnoreForUpdates = target;
        }
    }

    void GLOutput::AddTarget(GLInput* target, int textureLocation)
    {
        for (size_t i=0; i<_targets.size(); i++)
        {
            if (_targets[i] == target) return;
        }

        _cacheMaximumOutputSize = Size(0,0);
        SetInputFramebuffer(target, textureLocation);
        _targets.push_back(target);
        _targetTextureIndices.push_back(textureLocation);

        _allTargetsWantMonochromeData = _allTargetsWantMonochromeData && target->WantsMonochromeInput();
    }

    void GLOutput::RemoveTarget(GLInput* target)
    {
        bool found = false;
        size_t indexOfObject = 0;
        for (size_t i=0; i<_targets.size(); i++)
        {
            if (_targets[i] == target) {
                found = true;
                indexOfObject = i;
            }
        }
        if (!found) return;

        if (_targetToIgnoreForUpdates == target)
        {
            _targetToIgnoreForUpdates = NULL;
        }

        _cacheMaximumOutputSize = Size();
        //TODO:
        int textureIndexOfTarget = _targetTextureIndices[indexOfObject];

        target->SetInputSize(Size(0,0), textureIndexOfTarget);
        target->SetInputRotation(kGLNoRotation, textureIndexOfTarget);
        _targetTextureIndices.erase(_targetTextureIndices.begin()+indexOfObject);
        std::vector<GLInput*>::iterator it = _targets.find(target);
        if (it != _targets.end()) _targets.erase(it);
        target->EndProcessing();
    }

    void GLOutput::RemoveAllTargets(void)
    {
        Size size(0,0);
        _cacheMaximumOutputSize = size;

        for (size_t indexOfObject=0; indexOfObject<_targets.size(); i++)
        {
            int textureIndexOfTarget = _targetTextureIndices[indexOfObject];
            GLInput* target = _targets[indexOfObject];
            target->SetInputSize(size, textureIndexOfTarget);
            target->SetInputRotation(KGLNoRotation, textureIndexOfObject);
        }
        _allTargetsWantMonochromeData = true;
    }

    void GLOutput::SetOutputTextureOptions(GLTextureOptions outputTextureOptions)
    {
        _outputTextureOptions = outputTextureOptions;
        if (_outputFramebuffer->Texture())
        {
            glBindTexture(GL_TEXTURE_2D, _outputFramebuffer->Texture());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, _outputTextureOptions.wrapS);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, _outputTextureOptions.wrapT);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }

    void GLOutput::ForceProcessingAtSize(Size frameSize)
    {

    }

    void GLOutput::ForceProcessingAtSizeRespectingAspectRatio(Size frameSize)
    {

    }

    bool GLOutput::ShouldSmoothlyScaleOutput(void)
    {
        return _shouldSmoothlyScaleOutput;
    }

    bool GLOutput::ShouldIgnoreUpdatesToThisTarget(void)
    {
        return _shouldIgnoreUpdatesToThisTarget;
    }

    bool GLOutput::Enable(void)
    {
        return _enable;
    }
}