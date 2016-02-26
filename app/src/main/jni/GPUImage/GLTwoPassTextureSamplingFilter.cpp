//
// Created by lyf on 2016/2/26.
//

#include "GLTwoPassTextureSamplingFilter.h"


namespace e{

GLTwoPassTextureSamplingFilter::GLTwoPassTextureSamplingFilter(void)
{
    _verticalPassTexelWidthOffsetUniform  = 0;
    _verticalPassTexelHeightOffsetUniform = 0;
    _horizontalPassTexelWidthOffsetUniform = 0;
    _horizontalPassTexelHeightOffsetUniform = 0;

    _verticalPassTexelWidthOffset = 0;
    _verticalPassTexelHeightOffset = 0;
    _horizontalPassTexelWidthOffset = 0;
    _horizontalPassTexelHeightOffset = 0;

    _verticalTexelSpacing = 1.0f;
    _horizontalTexelSpacing = 1.0f;
}

GLTwoPassTextureSamplingFilter::~GLTwoPassTextureSamplingFilter(void)
{

}

//@initialize
bool GLTwoPassTextureSamplingFilter::Initialize(const char* firstStageVertexShaderString
    , const char* firstStageFragmentShaderString
    , const char* secondStageVertexShaderString
    , const char* secondStageFragmentShaderString)
{
    if (GLTwoPassFilter::Initialize(firstStageVertexShaderString
        , firstStageFragmentShaderString
        , secondStageVertexShaderString
        , secondStageFragmentShaderString))
    {
        return false;
    }

    GLContext::UseImageProcessingContext();
    _verticalPassTexelWidthOffsetUniform = _filterProgram->UniformIndex("texelWidthOffset");
    _verticalPassTexelHeightOffsetUniform = _filterProgram->UniformIndex("texelHeightOffset");
    _horizontalPassTexelWidthOffsetUniform = _secondFilterProgram->UniformIndex("texelWidthOffset");
    _horizontalPassTexelHeightOffsetUniform = _secondFilterProgram->UniformIndex("texelHeightOffset");

    _verticalTexelSpacing = 1.0f;
    _horizontalTexelSpacing = 1.0f;
    return true;
}

void GLTwoPassTextureSamplingFilter::SetUniforms(int programIndex)
{
    GLTwoPassFilter::SetUniforms(programIndex);

    if (programIndex == 0)
    {
        glUniform1f(_verticalPassTexelWidthOffsetUniform, _verticalPassTexelWidthOffset);
        glUniform1f(_verticalPassTexelHeightOffsetUniform, _verticalPassTexelHeightOffset);
    }
    else
    {
        glUniform1f(_horizontalPassTexelWidthOffsetUniform, _horizontalPassTexelWidthOffset);
        glUniform1f(_horizontalPassTexelHeightOffsetUniform, _horizontalPassTexelHeightOffset);
    }
}

void GLTwoPassTextureSamplingFilter::SetupFilter(Size size)
{
    if (GLRotationSwapsWidthAndHeight(_inputRotation))
    {
        _verticalPassTexelWidthOffset = _verticalTexelSpacing / size.cy;
        _verticalPassTexelHeightOffset = 0.0;
    }
    else
    {
        _verticalPassTexelWidthOffset = 0.0;
        _verticalPassTexelHeightOffset = _verticalTexelSpacing / size.cy;
    }

    _horizontalPassTexelWidthOffset = _horizontalTexelSpacing / size.cx;
    _horizontalPassTexelHeightOffset = 0.0;
}

void GLTwoPassTextureSamplingFilter::SetVerticalTexelSpacing(GLfloat value)
{
    _verticalTexelSpacing = value;
    SetupFilter(SizeOfFBO());
}

void GLTwoPassTextureSamplingFilter::SetHorizontalTexelSpacing(GLfloat value)
{
    _horizontalTexelSpacing = value;
    SetupFilter(SizeOfFBO());
}

}