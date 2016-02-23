//
// Created by yongfali on 2016/2/23.
//

#ifndef _E_GPUIMAGEFILTER_H_
#define _E_GPUIMAGEFILTER_H_

namespace e{

    extern char* kGPUImageVertexShaderString;
    extern char* kGPUImageFragmentShaderString;

    struct GPUVector4{
        GLfloat x,y,z,w;
    }
    typedef struct GPUVector4 GPUVector4;

    struct GPUVector3{
        GLfloat x,y,z;
    }
    typedef struct GPUVector3 GPUVector3;

    struct GPUMatrix4x4 {
        GPUVector4 x;
        GPUVector4 y;
        GPUVector4 z;
        GPUVector4 w;
    };
    typedef struct GPUMatrix4x4 GPUMatrix4x4;

    struct GPUMatrix3x3 {
        GPUVector3 x;
        GPUVector3 y;
        GPUVector3 z;
    };
    typedef struct GPUMatrix3x3 GPUMatrix3x3;

    class GPUImageFilter {
        public:
            GPUImageFilter(void);
            GPUImageFilter(const char* vertexShader, const char* fragmentShader);
            GPUImageFilter(const char* fragmentShader);
            virtual ~GPUImageFilter(void);

            void InitializeAttributes(void);
            void SetupFilterForSize(Size size);
            Size RotatedSize(Size sizeToRotate, int textureIndex);
            Point RotatedPoint(Point pointToRotate, GPUImageRotation rotation);
            Size SizeOfFBO(void);
            //rendering
            static const GLfloat* TextureCoordinate(GPUImageRotation rotationMode);
            void RenderToTexture(const GLfloat* vertices, const GLfloat* textureCoordinates);
            Size OutputFrameSize(void);
            //input parameters
            void SetBackgroundColor(GLfloat red, GLfloat green, GLfloat blue);
            void SetInteger(GLint value, const char* uniformName);
            void SetFloat(GLfloat value, const char* uniformName);
            void SetSize(Size value, const char* uniformName);
            void SetPoint(Point value, const char* uniformName);
            void SetFloatVec3(GPUVector3 value, const char* uniformName);
            void SetFloatVec4(GPUVector4 value, const char* uniformName);
            void SetFloatArray(GLfloat* array, int size, const char* uniformName);

            void SetMatrix3f(GPUMatrix3x3 matrix, GLint uniform, GLProgram* shaderProgram);
            void SetMatrix4f(GPUMatrix4x4 matrix, GLint uniform, GLProgram* shaderProgram);
            void SetFloat(GLfloat value, GLint uniform, GLProgram* shaderProgram);
            void SetPoint(Point value, GLint uniform, GLProgram* shaderProgram);
            void SetSize(Size value, GLint uniform, GLProgram* shaderProgram);
            void SetVec3(GPUVector3 value, GLint uniform, GLProgram* shaderProgram);
            void SetVec4(GPUVector4 value, GLint uniform, GLProgram* shaderProgram);
            void SetFloatArray(GLfloat* array, int count, GLint uniform, GLProgram* shaderProgram);
            void SetInteger(GLint value, GLint uniform, GLProgram* shaderProgram);

            void SetUniforms(int programIndex);
        protected:
            GPUImageFramebuffer* _firstInputFramebuffer;

            GLProgram* _filterProgram;
            GLint _filterPositionAttribute;
            GLint _filterTextureCoordinateAttribute;
            GLint _filterInputTextureUniform;
            GLfloat _backgroundColorRed;
            GLfloat _backgroundColorGreen;
            GLfloat _backgroundColorBlue;
            GLfloat _backgroundColorAlpha;

            bool _isEndProcessing;
            Size _currentFilterSize;

            GPUImageRotationMode _inputRotation;
            bool _currentlyReceivingMonochromeInput;
            sem_t _imageCaptureSemaphore;

            bool _preventRendering;

    };

}

#endif
