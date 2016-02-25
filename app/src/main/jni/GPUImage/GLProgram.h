//
// Created by yongfali on 2016/2/23.
//

#ifndef _E_GLPROGRAM_H_
#define _E_GLPROGRAM_H_
#include <GLES2/gl2.h>
#include <string.h>
#include <map>

using namespace std;

namespace e
{
    class GLProgram {
    public:
        GLProgram(void);
        GLProgram(const char* vertexShader, const char* fragmentShader);
        virtual ~GLProgram(void);

        bool InitializeWithString(const char* vertexShader, const char* fragmentShader);
        bool InitializeWithFilename(const char* vertexShader, const char* fragmentShader);
        void AddAttribute(const char* attrName);
        GLuint AttributeIndex(const char* attrName);
        GLuint UniformIndex(const char* attrName);
        bool Link(void);
        void Use(void);
        void Validate(void);

        bool Initialized(void) const;
        string ProgramLog(void) const;
        string VertexShaderLog(void) const;
        string FragmentShaderLog(void) const;
    private:
        bool CompileShader(GLuint* shader, GLenum type, const char* shaderString);
    private:
        GLuint _program;
        GLuint _vertexShader;
        GLuint _fragmentShader;

        string _programLog;
        string _vertexShaderLog;
        string _fragmentShaderLog;

        GLuint _attributeCount;
        std::map<string, GLuint> _attributes;
        std::map<string, GLuint> _uniforms;

        bool _initialized;
    };
}
#endif
