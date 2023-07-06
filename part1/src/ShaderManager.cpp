#include "ShaderManager.hpp"
#include "Utils.hpp"
#include <iostream>
#include "Scene.hpp"
#include "Shader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

ShaderManager::ShaderManager(int width, int height, bool fullScreen)
{
	this->width = width;
	this->height = height;
	this->fullScreen = fullScreen;
	this->useCustomResolution = true;
}

ShaderManager::~ShaderManager()
{
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);
	
	glDeleteTextures(1, &skyTex);
	//glDeleteTextures(textures.size(), textures.data());
}

int ShaderManager::getWidth()
{
	return width;
}

int ShaderManager::getHeight()
{
	return height;
}

GLuint ShaderManager::getProgramId()
{
	return shader.ID;
}

bool ShaderManager::init_window(){

	float quadVertices[] = 
	{
		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f,

		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f,
		-1.0f,  1.0f, 0.0f, 1.0f
	};

	// quad VAO
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glBindVertexArray(0);

	return true;
}

void ShaderManager::draw()
{
	shader.use();
	glBindVertexArray(quadVAO);
	
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	checkGlErrors("Draw raytraced image");
	
	shader.use();
}

void ShaderManager::gen_framebuffer(GLuint* fbo, GLuint* fboTex, GLenum internalFormat, GLenum format) const
{
	glGenFramebuffers(1, fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, *fbo);

	glGenTextures(1, fboTex);
	glBindTexture(GL_TEXTURE_2D, *fboTex);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *fboTex, 0);
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		exit(1);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShaderManager::init_shaders(rt_defines& defines) {
	const std::string vertexShaderSrc = readStringFromFile("./shaders/vert.glsl");
	std::string fragmentShaderSrc = readStringFromFile("./shaders/frag.glsl");
	
	replace(fragmentShaderSrc, "{SPHERE_SIZE}", std::to_string(defines.sphere_size));
	replace(fragmentShaderSrc, "{LIGHT_POINT_SIZE}", std::to_string(defines.light_point_size));
	replace(fragmentShaderSrc, "{LIGHT_DIRECT_SIZE}", std::to_string(defines.light_direct_size));
	replace(fragmentShaderSrc, "{ITERATIONS}", std::to_string(defines.iterations));
	replace(fragmentShaderSrc, "{AMBIENT_COLOR}", to_string(defines.ambient_color));
	replace(fragmentShaderSrc, "{SHADOW_AMBIENT}", to_string(defines.shadow_ambient));

	shader.initFromSrc(vertexShaderSrc.c_str(), fragmentShaderSrc.c_str());

	shader.use();

	checkGlErrors("Shader creation");
}

std::string ShaderManager::to_string(glm::vec3 v)
{
	return std::string().append("vec3(").append(std::to_string(v.x)).append(",").append(std::to_string(v.y)).append(",").append(std::to_string(v.z)).append(")");
}

unsigned int ShaderManager::load_cubemap(std::vector<std::string> faces, bool genMipMap){
    unsigned int textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

    int width, height, numChannels;
    for (int i=0; i<faces.size(); i++){
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &numChannels, 0);
        if (data){
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                         GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    if (genMipMap) {
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, genMipMap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureId;
}

void ShaderManager::set_background(unsigned backgroundId){
    skyTex = backgroundId;
    shader.setInt("background", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyTex);
}

void ShaderManager::init_buffer(GLuint* ubo, const char* name, int bindingPoint, size_t size, void* data) const
{
	glGenBuffers(1, ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, *ubo);
	glBufferData(GL_UNIFORM_BUFFER, size, data, GL_DYNAMIC_DRAW);
	GLuint blockIndex = glGetUniformBlockIndex(shader.ID, name);
	if (blockIndex == 0xffffffff)
	{
		fprintf(stderr, "Invalid ubo block name '%s'\n", name);
		exit(1);
	}
	glUniformBlockBinding(shader.ID, blockIndex, bindingPoint);
	glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, *ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void ShaderManager::update_buffer(GLuint ubo, size_t size, void* data)
{
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, size, data);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
