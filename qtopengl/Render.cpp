#include "Render.h"

// Shaders
const GLchar* g_pVertexShaderSource = "#version 440 core\n"
"layout (location = 0) in vec3 vertexIn;\n"
"layout (location = 1) in vec2 textureIn;\n"
"varying vec2 textureOut;\n"
"void main(void)\n"
"{\n"
"	gl_Position = vec4(vertexIn, 1.0);\n"
"	textureOut = textureIn;\n"
"}\n";
//yuv420p to rgb
const GLchar* g_pFragmentShaderSource = "#version 440 core\n"
"varying vec2 textureOut;\n"
"uniform sampler2D texy;\n"
"uniform sampler2D texu;\n"
"uniform sampler2D texv;\n"
"void main(void)\n"
"{\n"
"	vec3 yuv;\n"
"	vec3 rgb;\n"
"	yuv.x = texture2D(texy, textureOut).r;\n"
"	yuv.y = texture2D(texu, textureOut).r - 0.5;\n"
"	yuv.z = texture2D(texv, textureOut).r - 0.5;\n"
"	rgb = mat3(1, 1, 1,\n"
"		0, -0.39465, 2.03211,\n"
"		1.13983, -0.58060, 0) * yuv;\n"
"	//gl_FragColor = vec4(0.0,0.0,1.0, 1);\n"
"	gl_FragColor = vec4(rgb, 1);\n"
"}\n";

//nv12 to rgb
const GLchar* g_pFragmentShaderSource_nv12torgb = "#version 440 core\n"
"varying vec2 textureOut;\n"
"uniform sampler2D texy;\n"
"uniform sampler2D texuv;\n"
"void main(void)\n"
"{\n"
"	vec3 yuv;\n"
"	vec3 rgb;\n"
"	yuv.x = texture2D(texy, textureOut).r;\n"
"	yuv.yz = texture2D(texuv, textureOut) - vec2(0.5, 0.5);\n"
"	rgb = mat3(1, 1, 1,\n"
"		0, -0.39465, 2.03211,\n"
"		1.13983, -0.58060, 0) * yuv;\n"
"	gl_FragColor = vec4(rgb, 1);\n"
"}\n";
//rgb24 to 32
const GLchar* g_pFragmentShaderSource_rgb24torgb32 = "#version 440 core\n"
"varying vec2 textureOut;\n"
"uniform sampler2D texy;\n"
"void main(void)\n"
"{\n"
"	vec3 rgb;\n"
"	rgb = texture2D(texy, textureOut);\n"
"	gl_FragColor = vec4(rgb, 1);\n"
"}\n";

Render::Render()
{
}


Render::~Render()
{
	wglMakeCurrent(renderDC_, NULL);
	wglDeleteContext(glRC_);
}
void Render::SetViewport()
{
	RECT rect;
	GetWindowRect(renderHandle_, &rect);
	glViewport(0, 0, rect.right - rect.left, rect.bottom - rect.top);
}
int Render::Init(int width, int height, HWND handle, VideoFormat fmt)
{
	renderHandle_ = handle;
	renderDC_ = GetWindowDC(handle);
	width_ = width;// rect.right - rect.left;
	height_ = height;// rect.bottom - rect.top;
	if (!InitRenderContext())
		return -1;
	SetViewport();
    if (!CreateShaderProgram(width_, height_, fmt)) {
        return -2;
    }
	InitVertex();
	return 0;
}
bool Render::InitVertexShader()
{
	// Build and compile our shader program
	// Vertex shader
	vertexShader_ = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader_, 1, &g_pVertexShaderSource, NULL);
	glCompileShader(vertexShader_);
	// Check for compile time errors
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader_, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader_, 512, NULL, infoLog);
		std::cerr<< "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		return false;
	}
	return true;
}
bool Render::InitFragmentShader(const GLchar ** _shaderSourceCode)
{
	// Fragment shader
	fragmentShader_ = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader_, 1, _shaderSourceCode, NULL);
	glCompileShader(fragmentShader_);
	// Check for compile time errors
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(fragmentShader_, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader_, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		return false;
	}
	return true;
}
bool Render::CreateShaderProgram(int width, int height, VideoFormat format)
{
	// Link shaders
	shaderProgram_ = glCreateProgram();
	if (!InitVertexShader())
		return false;
	switch (format)
	{
	case YUV420P:
		if (!InitFragmentShader(&g_pFragmentShaderSource))
			return false;
        CreateYUV420PTexture();
		break;
	case NV12:
		if (!InitFragmentShader(&g_pFragmentShaderSource_nv12torgb))
			return false;
        CreateNV12Texture();
		break;
	case RGB24:
		if (!InitFragmentShader(&g_pFragmentShaderSource_rgb24torgb32))
			return false;
        CreateRGB24Texture();
		break;
	case RGBA:
		break;
	default:
		break;
	}
	glAttachShader(shaderProgram_, vertexShader_);
	glAttachShader(shaderProgram_, fragmentShader_);
	glLinkProgram(shaderProgram_);
	// Check for linking errors
	GLint success;
	GLchar infoLog[512];
	glGetProgramiv(shaderProgram_, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram_, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		return false;
	}
	glDeleteShader(vertexShader_);
	glDeleteShader(fragmentShader_);
	return true;
}
void Render::InitVertex()
{
	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] = {
		/*
		//left down triangle
		-1.0f, -1.0f, 0.0f,    0.0f, 0.0f, //left down
		1.0f, -1.0f, 0.0f,     1.0f, 0.0f, //right down
		1.0f, 1.0f, 0.0f,      1.0f, 1.0f, //right up
		//right top triangle
		-1.0f, -1.0f, 0.0f,     0.0f, 0.0f, //left down
		1.0f, 1.0f, 0.0f,       1.0f, 1.0f, //right up
		-1.0f, 1.0f, 0.0f,      0.0f, 1.0f, //left up
		*/
		//left down triangle
		-1.0f, -1.0f, 0.0f,    1.0f, 1.0f, //left down
		1.0f, -1.0f, 0.0f,     0.0f, 1.0f, //right down
		1.0f, 1.0f, 0.0f,      0.0f, 0.0f, //right up
		//right top triangle
		-1.0f, -1.0f, 0.0f,     1.0f, 1.0f, //left down
		1.0f, 1.0f, 0.0f,       0.0f, 0.0f, //right up
		-1.0f, 1.0f, 0.0f,      1.0f, 0.0f, //left up
	};
	glGenVertexArrays(1, (GLuint *)&VAO);
	glGenBuffers(1, (GLuint*)&VBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	int location = glGetAttribLocation(shaderProgram_, "vertexIn");
	glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(location);

	location = glGetAttribLocation(shaderProgram_, "textureIn");
	glVertexAttribPointer(location, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(location);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)
}
bool Render::InitRenderContext(){
	// Set surface pixel format
	PIXELFORMATDESCRIPTOR pixel_desc;
	pixel_desc.nSize = sizeof(pixel_desc); //WORD  nSize;
	pixel_desc.nVersion = 1; //WORD  nVersion;
	pixel_desc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;//DWORD dwFlags;
	pixel_desc.iPixelType = PFD_TYPE_RGBA; //BYTE  iPixelType;
	pixel_desc.cColorBits = 32; //BYTE  cColorBits;
	pixel_desc.cRedBits = 0;//BYTE  cRedBits;
	pixel_desc.cRedShift = 0;//BYTE  cRedShift;
	pixel_desc.cGreenBits = 0;//BYTE  cGreenBits;
	pixel_desc.cGreenShift = 0;//BYTE  cGreenShift;
	pixel_desc.cBlueBits = 0;//BYTE  cBlueBits;
	pixel_desc.cBlueShift = 0;//BYTE  cBlueShift;
	pixel_desc.cAlphaBits = 0;//BYTE  cAlphaBits;
	pixel_desc.cAlphaShift = 0;//BYTE  cAlphaShift;
	pixel_desc.cAccumBits = 0;//BYTE  cAccumBits;
	pixel_desc.cAccumRedBits = 0;//BYTE  cAccumRedBits;
	pixel_desc.cAccumGreenBits = 0;//BYTE  cAccumGreenBits;
	pixel_desc.cAccumBlueBits = 0;//BYTE  cAccumBlueBits;
	pixel_desc.cAccumAlphaBits = 0;//BYTE  cAccumAlphaBits;
	pixel_desc.cDepthBits = 24;//BYTE  cDepthBits;
	pixel_desc.cStencilBits = 8;//BYTE  cStencilBits;
	pixel_desc.cAuxBuffers = 0;//BYTE  cAuxBuffers;
	pixel_desc.iLayerType = PFD_MAIN_PLANE;//BYTE  iLayerType;
	pixel_desc.bReserved = 0;//BYTE  bReserved;
	pixel_desc.dwLayerMask = 0;//DWORD dwLayerMask;
	pixel_desc.dwVisibleMask = 0;//DWORD dwVisibleMask;
	pixel_desc.dwDamageMask = 0;//DWORD dwDamageMask;

	int pixel_fmt = ChoosePixelFormat(renderDC_, &pixel_desc);
	if (pixel_fmt == 0) {
		MessageBox(NULL, NULL, L"Error", MB_OK);
		return false;
	}

	if (SetPixelFormat(renderDC_, pixel_fmt, &pixel_desc) == FALSE) {
		MessageBox(NULL, NULL, L"Error", MB_OK);
		return false;
	}

	int n = GetPixelFormat(renderDC_);
	DescribePixelFormat(renderDC_, n, sizeof(pixel_desc), &pixel_desc);

	// Create opengl render context
	glRC_ = wglCreateContext(renderDC_);
	if (glRC_ == NULL) {
		MessageBox(NULL, NULL, L"Error", MB_OK);
		return false;
	}

	wglMakeCurrent(renderDC_, glRC_);
	glewInit();
	return true;
}
void Render::CreateTexture(GLuint * tex)
{
	glGenTextures(1, tex);
	glBindTexture(GL_TEXTURE_2D, *tex); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object
	// Set our texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);    // Note that we set our container wrapping method to GL_CLAMP_TO_EDGE
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);    // Note that we set our container wrapping method to GL_CLAMP_TO_EDGE
	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
}
void Render::CreateRGB24Texture()
{
	CreateTexture(&texture_);
}
void Render::CreateNV12Texture()
{
	CreateTexture(&texture_);
	CreateTexture(&textureuv_);
}
void Render::CreateYUV420PTexture()
{
	CreateTexture(&texture_);
	CreateTexture(&textureu_);
	CreateTexture(&texturev_);
}
FILE *infile = NULL;
unsigned char data[1280*720* 4];

void Render::Draw()
{
	unsigned char *plane[3];
	plane[0] = data;
	plane[1] = data + width_ * height_;
	plane[2] = data + width_ * height_ * 5 / 4;
	if (infile == NULL){
		infile = fopen("test_yuv420p_320x180.yuv", "rb");
		//infile = fopen("nv12.yuv", "rb");
	}
REREAD:
	if (fread(data, 1, width_*height_* 3 / 2, infile) != width_*height_* 3 / 2){
		// Loop
		fseek(infile, 0, SEEK_SET);
        goto REREAD;
	}

	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shaderProgram_);
	glBindVertexArray(VAO[0]);

	//Y
	int location = glGetUniformLocation(shaderProgram_, "texy");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width_, height_, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, plane[0]);
	glUniform1i(location, 0);
	//U
	//GL_RED->GL_LUMINANCE;
	location = glGetUniformLocation(shaderProgram_, "texu");
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureu_);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width_/ 2, height_/ 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, plane[1]);
	glUniform1i(location, 1);
	//V
	location = glGetUniformLocation(shaderProgram_, "texv");
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texturev_);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width_/ 2, height_/ 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, plane[2]);
	glUniform1i(location, 2);

	// Draw our first triangle
	glDrawArrays(GL_TRIANGLES, 0, 6);

    glFlush();
	SwapBuffers(renderDC_);
	glBindVertexArray(0);
}
