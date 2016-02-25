//
// Created by yongfali on 2016/2/25.
//

#ifndef _E_GLOUTPUT_H_
#define _E_GLOUTPUT_H_

namespace e{

class GLOutput {
public:
    GLOutput(void);
    virtual ~GLOutput(void);

    void SetInputFramebuffer(GLInput* target, int inputTextureIndex);
    GLFramebuffer* FramebufferForOutput(void);
    void RemoveOutputFramebuffer(void);
    void NotifyTargetsAboutNewOutputTexture(void);

    bool ShouldSmoothlyScaleOutput(void);
    bool ShouldIgnoreUpdatesToThisTarget(void);
    bool Enable(void);

    std::vector<GLInput*> Targets(void);
    void AddTarget(GLInput* target);
    void AddTarget(GLInput* target, int textureLocation);
    void RemoveTarget(GLInput* target);
    void RemoveAllTargets(void);

    void ForceProcessingAtSize(Size frameSize);
    void ForceProcessingAtSizeRespectingAspectRatio(Size frameSize);

    bool ProvidesMonochromeOutput(void);

protected:
    GLFramebuffer* _outputFramebuffer;
    Size _inputTextureSize;
    Size _cachedMaximumOutputSize;
    Size _forcedMaximumSize;

    bool _overrideInputSize;
    bool _allTargetsWantMonochromeData;
    bool _usingNextFrameForImageCapture;

    bool _shouldSmoothlyScaleOutput;
    bool _shouldIgnoreUpdatesToThisTarget;
    bool _enable;
    GLInput* _targetToIgnoreForUpdates;
    GLTextureOptions _outputTextureOptions;

    std::vector<GLInput*> _targets;
    std::vector<int> _targetTextureIndices;
};

}

#endif
