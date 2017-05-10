#include "qtopengl.h"
// Shaders
const GLchar* vertexShaderSource = "#version 440 core\n"
"layout (location = 0) in vec3 position;\n"
"void main()\n"
"{\n"
"gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
"}\0";
const GLchar* fragmentShaderSource = "#version 440 core\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"
"}\n\0";
const GLchar* fragmentShader2Source = "#version 330 core\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = vec4(1.0f, 1.0f, 0.0f, 1.0f); // The color yellow \n"
"}\n\0";

qtopengl::qtopengl(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
}
qtopengl::~qtopengl(){
	wglMakeCurrent(m_hdc, NULL);
	wglDeleteContext(m_glrc);
	//ReleaseDC((HWND£©ui->label.winId(), m_hdc);
}
void qtopengl::init(){
	m_hdc = GetDC((HWND)ui.label->winId());

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

	int pixel_fmt = ChoosePixelFormat(m_hdc, &pixel_desc);
	if (pixel_fmt == 0) {
		MessageBox(NULL, NULL, L"Error", MB_OK);
		return;
	}

	if (SetPixelFormat(m_hdc, pixel_fmt, &pixel_desc) == FALSE) {
		MessageBox(NULL, NULL, L"Error", MB_OK);
		return;
	}

	int n = GetPixelFormat(m_hdc);
	DescribePixelFormat(m_hdc, n, sizeof(pixel_desc), &pixel_desc);

	// Create opengl render context
	m_glrc = wglCreateContext(m_hdc);
	if (m_glrc == NULL) {
		MessageBox(NULL, NULL, L"Error", MB_OK);
		return;
	}
	wglMakeCurrent(m_hdc, m_glrc);

	// Init glew
	glewInit();
	glViewport(0, 0, ui.label->width(), ui.label->height());
	glClearColor(0.0f, 1.0f, 0.0f, 1.0f);


	// Build and compile our shader program
	// Vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// Check for compile time errors
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		exit(2);
	}
	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// Check for compile time errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		exit(3);
	}
	// Fragment shader
	GLuint fragment2Shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment2Shader, 1, &fragmentShader2Source, NULL);
	glCompileShader(fragment2Shader);
	// Check for compile time errors
	glGetShaderiv(fragment2Shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment2Shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		exit(4);
	}
	// Link shaders
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// Check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	// Link shaders
	shader2Program = glCreateProgram();
	glAttachShader(shader2Program, vertexShader);
	glAttachShader(shader2Program, fragment2Shader);
	glLinkProgram(shader2Program);
	// Check for linking errors
	glGetProgramiv(shader2Program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shader2Program, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteShader(fragment2Shader);


	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices1[] = {
		-1.0f, -0.5f, 0.0f, // Left  
		0.0f, -0.5f, 0.0f, // Right 
		-0.5f, 0.5f, 0.0f,  // Top   
	};
	GLfloat vertices2[] = {
		0.0f, -0.5f, 0.0f, // Left  
		1.0f, -0.5f, 0.0f, // Right 
		0.5f, 0.5f, 0.0f  // Top   
	};
	glGenVertexArrays(2, (GLuint *)&VAO);
	glGenBuffers(2, (GLuint*)&VBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);


	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);


	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)

}

void qtopengl::draw() {
	/*
	GLfloat clear_c[4] = { rand() % 100 / 100.0f, rand() % 100 / 100.0f
		, rand() % 100 / 100.0f, 1.0f };
	glClearColor(clear_c[0], clear_c[1], clear_c[2], 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();
	SwapBuffers(m_hdc);
	*/
	// Render
	// Clear the colorbuffer
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Draw our first triangle
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glUseProgram(shader2Program);
	glBindVertexArray(VAO[1]);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glFlush();
	SwapBuffers(m_hdc);


	glBindVertexArray(0);
}
void qtopengl::start() {
	timer.start(40);
	connect(&timer, SIGNAL(timeout()), this, SLOT(draw()));
}

void qtopengl::glstart(){
	auto size = ui.label->size();
	if (render_.Init(320, 180, (HWND)ui.label->winId(), VideoFormat::YUV420P)){
		qDebug() << "----------------------->init fail";
		return;
	}
	timer.start(40);
	connect(&timer, SIGNAL(timeout()), this, SLOT(gldraw()));
}
void qtopengl::gldraw(){
	render_.Draw();
}