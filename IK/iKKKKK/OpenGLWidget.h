#pragma once

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QWindow>
#include <QScreen>
#include <QOpenGLFunctions>
#include <QPainter>
#include <QOpenGLPaintDevice>
#include <QOpenGLShaderProgram>
#include <QPushButton>
#include <QOpenGLWidget>
#include <QTimer>

#include "BVH.h"
#include "stdafx.h"



class OpenGLWidget : public QOpenGLWidget, public QOpenGLFunctions
{
Q_OBJECT
public:
	
	// parameters
	GLint m_posAttr = 0;
	GLint m_colAttr = 0;
	GLint m_matrixUniform = 0;
	QOpenGLFunctions* f;
	double frameTime = 0.5;
	int frameNum = 0;
	QTimer *timer = new QTimer(this);

	const char* vertexShaderSource =
		"attribute highp vec4 posAttr;\n"
		"attribute lowp vec4 colAttr;\n"
		"varying lowp vec4 col;\n"
		"uniform highp mat4 matrix;\n"
		"void main() {\n"
		"   col = colAttr;\n"
		"   gl_Position = matrix * posAttr;\n"
		"}\n";

	const char* fragmentShaderSource =
		"varying lowp vec4 col;\n"
		"void main() {\n"
		"   gl_FragColor = col;\n"
		"}\n";


	QOpenGLShaderProgram* m_program = nullptr;
	

	// flag
	int animationOn = 0;
	int nextFrameOn = 0;


	// constructor
	OpenGLWidget(QWidget* parent = nullptr);

	// function
	void drawCylinder(float radius, float length, float x, float y, float z);
	void playAnimation(int flag);
	void drawNextFrame();
	void setFrame(int _frameNum);
	int checkFrameNum();
	virtual void render();
	

protected:

	

	// function
	void initializeGL() override;
	void resizeGL(int w, int h) override;
	void paintGL() override;


private slots:
	void checkTime();

};

