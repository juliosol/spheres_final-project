/**
 * @file Shader.hpp
 * @author Julio Soldevilla
 * @brief Manages the loading, compiling and linking of vertex and fragment shaders
 * Additionally has functions for setting various uniforms.
 * 
 */

#ifndef SHADER_HPP
#define SHADER_HPP

#if defined(LINUX) || defined(MINGW)
    #include <SDL2/SDL.h>
#else 
    #include <SDL2/SDL.h>
#endif

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
//#include "utils.h"

class Shader {
    public:
        GLuint ID;

        Shader();
        
        Shader(const std::string& vertexPath, const std::string& fragmentPath);

        Shader(const char* vertexPath, const char* fragmentPath);
        
        ~Shader();

        void initFromFile(const char* vertexPath, const char* fragmentPath);

        void initFromSrc(const std::string& vertexSrc, const std::string& fragmentSrc);
        
        void initFromSrc(const char* vertexSrc, const char* fragmentSrc);

        void use();

        void setBool(const std::string& name, bool value) const;
        void setInt(const std::string& name, int value) const;
        void setFloat(const std::string& name, float value) const;
        void setVec2(const std::string& name, const glm::vec2& value) const;
        void setVec2(const std::string& name, float x, float y) const;
        void setVec3(const std::string& name, const glm::vec3& value) const;
        void setVec3(const std::string& name, float x, float y, float z) const;
        void setVec4(const std::string& name, const glm::vec4& value) const;
        void setVec4(const std::string& name, float x, float y, float z, float w) const;
        void setMat2(const std::string& name, const glm::mat2& mat) const;
        void setMat3(const std::string& name, const glm::mat3& mat) const;
        void setMat4(const std::string& name, const glm::mat4& mat) const;

    private:
        // utility function for checking shader compilation/linking errors.
        void checkCompileErrors(unsigned int shader, std::string type);
};

/*
class Shader{
    public:
        //Shader constructor
        Shader();
        // Shader destructor
        ~Shader();
        // Use this shader in our pipeline
        void Bind() const;
        // Remove shader from our pipeline
        void Unbind() const;
        // Load a shader
        std::string LoadShader(const std::string& fname);
        // Create a Sahder from a loaded vertex and fragment shader
        void CreateShader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
        // Return the shader id
        GLuint GetID() const;
        // Set our unifroms for our shader
        void SetUniformMatrix4fv(const GLchar* name, const GLfloat* value);
        void SetUniform3f(const GLchar* name, float v0, float v1, float v2);
        void SetUniform1i(const GLchar* name, int value);
        void SetUniform1f(const GLchar* name, float value);
    
    private:
        // Compiles loaded shaders
        unsigned int CompileShader(unsigned int type, const std::string& source);
        // Makes sure shaders 'linked' successfully
        bool CheckLinkStatus(GLuint programID);
        // Shader loading utility program
        void PrintProgramLog(GLuint program);
        void PrintShaderLog(GLuint shader);
        // Logs an error message
        void Log(const char* system, const char* message);
        // The unique shaderID
        GLuint m_shaderID;
};

*/
#endif