#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_qtopengl.h"
#include <QTimer>
#include <QDebug>
/*
#include <Windows.h>
#include <iostream>
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
*/

#include "Render.h"

class qtopengl : public QMainWindow
{
	Q_OBJECT

public:
	qtopengl(QWidget *parent = Q_NULLPTR);
	~qtopengl();
	void init();
	HDC m_hdc;
	HGLRC m_glrc;
	void start();
	void glstart();

private:
	Ui::qtopenglClass ui;
	QTimer timer;
	GLuint shaderProgram;
	GLuint shader2Program;
	GLuint VBO[2], VAO[2];
	Render render_;
private slots:
	void draw();
	void gldraw();
};
