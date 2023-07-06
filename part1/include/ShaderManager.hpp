#pragma once

#include <glad/glad.h>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "Shader.hpp"

struct rt_defines;

class ShaderManager
{
public:
	ShaderManager(int width, int height, bool fullScreen);
	//GLWrapper(bool fullScreen);
	~ShaderManager();

	int getWidth();
	int getHeight();
	GLuint getProgramId();

	bool init_window();
	void init_shaders(rt_defines& defines);
	void set_background(unsigned int textureId);
	unsigned int load_cubemap(std::vector<std::string> faces, bool genMipMap);

	void draw();
	//static GLuint load_cubemap(std::vector<std::string> faces, bool genMipmap = false);
	//GLuint load_texture(int texNum, const char* name, const char* uniformName, GLuint wrapMode = GL_REPEAT);
	void init_buffer(GLuint* ubo, const char* name, int bindingPoint, size_t size, void* data) const;
	static void update_buffer(GLuint ubo, size_t size, void* data);

private:
	Shader shader;
	GLuint skyTex;
	GLuint quadVAO, quadVBO;
	std::vector<GLuint> textures;

	int width;
	int height;

	bool fullScreen = true;
	bool useCustomResolution = false;
	
	void gen_framebuffer(GLuint* fbo, GLuint* fboTex, GLenum internalFormat, GLenum format) const;
	
	static std::string to_string(glm::vec3 v);
};

