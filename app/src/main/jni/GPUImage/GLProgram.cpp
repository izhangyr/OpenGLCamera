//
// Created by yongfali on 2016/2/23.
//

#include "GLProgram.h"
#include <memory.h>
#include <assert.h>

namespace
 {
        GLProgram::GLProgram(void)
        {
            _program = 0;
            _vertexShader = 0;
            _fragmentShader = 0;
            _attributeCount = 0;
            _initialized = false;
        }

        GLProgram::GLProgram(const char* vertexShader, const char* fragmentShader)
        {
            _program = 0;
            _vertexShader = 0;
            _fragmentShader = 0;
            _attributeCount = 0;
            _initialized = false;
            //init with shader string
            bool ret = InitializeWithString(vertexShader, fragmentShader);
            assert(ret);
        }

        GLProgram::~GLProgram(void)
        {
            if (_vertexShader) glDeleteShader(_vertexShader);
            if (_fragmentShader) glDeleteShader(_fragmentShader);
            if (_program) glDeleteProgram(_program);
        }

        bool GLProgram::InitializeWithString(const char* vertexShader, const char* fragmentShader)
        {
            _initialized = false;
            _program = glCreateProgram();
            if (!CompileShader(&_vertexShader, GL_VERTEX_SHADER, vertexShader))
            {
                return false;
            }

            if (!CompileShader(&_fragmentShader, GL_FRAGMENT_SHADER, fragmentShader))
            {
                return false;
            }

            glAttachShader(_program, _vertexShader);
            glAttachShader(_program, _framentShader);
            return true;
        }

        bool GLProgram::InitializeWithFilename(const char* vertexShader, const char* fragmentShader)
        {
            return false;
        }

        bool GLProgram::CompileShader(GLuint* shader, GLenum type, const char* shaderString)
        {
            *shader = glCreateShader();
            glShaderSource(shader, 1, shaderString, NULL);
            glCompileShader(shader);

           GLint status = -1;
           glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);

           if (status != GL_TRUE)
           {
                GLint logLength = 0;
                glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logLength);
                if (logLength > 0)
                {
                    GLchar* log = (GLchar*)malloc(logLength+1);
                    memset(log, 0, logLength+1);
                    glGetShaderInfoLog(*shader, logLength, &logLength, log);
                    if (type == GL_VERTEX_SHADER)
                    {
                        _vertexShaderLog = log;
                    }
                    else
                    {
                        _fragmentShaderLog = log;
                    }

                    free(log);
                }
           }

           return status == GL_TRUE;
        }

        bool GLProgram::Link(void)
        {
            GLint status = 0;
            glLinkProgram(_program);
            glGetProgramiv(_program, GL_LINK_STATUS, &status);
            if (status == GL_FALSE) return false;

            if (_vertexShader){
                glDeleteShader(_vertexShader);
                _vertexShader = 0;
            }

            if (_fragmentShader){
                glDeleteShader(_fragmentShader);
                _fragmentShader = 0;
            }

            _initialized = true;
            return true;
        }

        void GLProgram::Use(void)
        {
            glUseProgram(_program);
        }

        void GLProgram::Validate(void)
        {
            GLint logLength;
            glValidateProgram(_program);
            glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &logLength);
            if (logLength > 0)
            {
                GLchar* log = (GLchar*)malloc(logLength+1);
                memset(log, 0, logLength+1);
                glGetProgramInfoLog(_program, logLength, &LogLength, log);
                this._programLog = log;
                free(log);
            }
        }

        void GLProgram::AddAttribute(const char* attrName)
        {
            std::map<string, GLuint>::iterator it = _attributes.find(string(attrName));
            if (it == _attributes.end())
            {
                GLuint index = _attributeCount++;
                _attributes.insert(pair<sting,GLuint>(attrName, index));
                glBindAttribLocation(_program,index);
            }
        }

        GLuint GLProgram::AttributeIndex(const char* attrName)
        {
            std::map<string, GLuint>::iterator it = _attributes.find(string(attrName));
            if (it != _attributes.end())
            {
                return it->second;
            }
            else
            {
                return (GLuint)(-1);
            }
        }

        GLuint GLProgram::UniformIndex(const char* attrName)
        {
            return glGetUniformLocation(_program, attrName);
        }

        bool GLProgram::Initialized(void) const
        {
            return _initialized;
        }

        string GLProgram::ProgramLog(void) const
        {
            return _programLog;
        }

        string GLProgram::VertexShaderLog(void) const
        {
            return _vertexShaderLog;
        }

        string GLProgram::FragmentShaderLog(void) const
        {
            return _fragmentShaderLog;
        }
}