#ifndef RENDERER_H
#define RENDERER_H
#include <glad/glad.h>
#include "Shader.h"
#include <iostream>
class Renderer {
public:
	static Renderer* m_Instance;
	static Renderer* Instance();
	void start();
	void stop();
	void render(GLenum, GLint, GLsizei);
	void updateQuadShader(Shader *);
	
private:
	unsigned int m_fbo;
	Renderer();

	//normal, diffuse, position, tex
	GLuint m_textures[4];
	GLuint m_depthTexture;
	GLuint renderedTexture;
	GLuint renderedTexture1;
	GLuint renderedTexture2;
	GLuint renderedTexture3;
	GLuint depthrenderbuffer;
	GLuint renderbuffer;
	GLuint framebuffer, texture, texture2;

	enum GBUFFER_TEXTURE_TYPE {
		GBUFFER_TEXTURE_TYPE_POSITION,
		GBUFFER_TEXTURE_TYPE_DIFFUSE,
		GBUFFER_TEXTURE_TYPE_NORMAL,
		GBUFFER_TEXTURE_TYPE_TEXCOORD,
		GBUFFER_NUM_TEXTURES
	};
};

#endif