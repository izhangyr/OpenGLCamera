//
// Created by yongfali on 2016/2/25.
//

#ifndef OPENGLCAMERA_GLTWOPASSFILTER_H
#define OPENGLCAMERA_GLTWOPASSFILTER_H

namespace e {

class GLTwoPassFilter
    : public GLShaderFilter{
public:
    bool Initialize(const char* firstStageVertexShaderString
        , const char* firstStageFragmentShaderString
        , const char* secondStageVertexShaderString
        , const char* secondStageFragmentShaderString);
    bool Initialize(const char* firstStageFragmentShaderString,  const char* secondStageFragmentShaderString);
    void InitializeSecondaryAttributes(void);
    GLFramebuffer* FramebufferForOutput(void);
    void RemoveOutputFramebuffer(void);
    virtual void SetUniforms(int programIndex);
protected:
    GLFramebuffer* _secondOutputFramebuffer;
    GLProgram* _secondFilterProgram;
    GLint _secondFilterPositionAttribute;
    GLint _secondFilterTextureCoordinateAttribute;
    GLint _secondFilterInputTextureUniform;
    GLint _secondFilterInputTextureUniform2;
};

}

#endif //OPENGLCAMERA_GLTWOPASSFILTER_H
