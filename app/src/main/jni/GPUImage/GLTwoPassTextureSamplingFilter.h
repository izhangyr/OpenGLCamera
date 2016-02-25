//
// Created by lyf on 2016/2/26.
//

#ifndef _E_GLTWOPASSTEXTURESAMPLINGFILTER_H_
#define _E_GLTWOPASSTEXTURESAMPLINGFILTER_H_

namespace e{

class GLTwoPassTextureSamplingFilter
    : public GLTwoPassFilter{
public:
    GLTwoPassTextureSamplingFilter(void);
    virtual ~GLTwoPassTextureSamplingFilter(void);
protected:
    GLint _verticalPassTexelWidthOffsetUniform;
    GLint _verticalPassTexelHeightOffsetUniform;
    GLint _horizontalPassTexelWidthOffsetUniform;
    GLint _horizontalPassTexelHeightOffsetUniform;

    GLfloat _verticalPassTexelWidthOffset;
    GLfloat _verticalPassTexelHeightOffset;
    GLfloat _horizontalPassTexelWidthOffset;
    GLfloat _horizontalPassTexelHeightOffset;

    GLFloat _verticalTexelSpacing;
    GLfloat _horizontalTexelSpacing;
};

}

#endif //_E_GLTWOPASSTEXTURESAMPLINGFILTER_H_
