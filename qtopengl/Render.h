#pragma once
#include <windows.h>
#include <iostream>
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
enum VideoFormat
{
	YUV420P,
	NV12,
	RGB24,
	RGBA,
};
class Render
{
public:
	/*
	 * @width: video width
	 * @height: video wideo height 
	 * @handle: video present on
	 */
	int Init(int width, int height, HWND handle, VideoFormat);
	Render();
	~Render();
	void Draw();

private:
	bool InitRenderContext();
	void SetViewport();
	bool InitVertexShader();
	bool InitFragmentShader(const GLchar ** shaderSourceCode_);
	//InitShaderResourceView(UINT width, UINT height, BOOL isRGBA);
	bool CreateShaderProgram(int width, int height, VideoFormat format);
	void InitVertex();
	void CreateYUV420PTexture();
	void CreateNV12Texture();
	void CreateRGB24Texture();
	void CreateTexture(GLuint * tex);
	HWND renderHandle_;
	HDC renderDC_;
	HGLRC glRC_;
	int width_;
	int height_;
	GLuint vertexShader_;
	GLuint fragmentShader_;
	GLuint shaderProgram_;
	GLuint VAO[1];
	GLuint VBO[1];
	GLuint texture_;
	GLuint textureu_;
	GLuint texturev_;
	GLuint textureuv_; //texturevu_
};
