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
#include <QFileDialog>

#include <QMouseEvent>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>


#include "ReadbvhFile.h"
#include "MoveFigure.h"


#include <iostream>
#include <qDebug>
#include <fstream>
#include <iomanip>

class MyMainWindow : public QMainWindow {

public:
// parameter
	
	// for read and write bvh file
	ReadbvhFile *openglwidget;
	MoveFigure *movefigure;


	// ------------- Widgets------------- //

	// buttons
	QPushButton* selectFile = new QPushButton("Display BVH file", this);
	QPushButton* createAnimation = new QPushButton("Create Animation", this);
	QPushButton* createFrame = new QPushButton("Add Frame", this);
	QPushButton* generateFile = new QPushButton("Generate New BVH File", this);

	// check box
	QCheckBox* damped = new QCheckBox("Damped",this);

	// slider
	QSlider* lambdaValue = new QSlider(this);

	// line editor
	QLineEdit* speed = new QLineEdit(this);
	
	// label
	QLabel* forLambda = new QLabel("Lambda for damping:", this);
	QLabel* forFrameTime = new QLabel("Frame Time:", this);


	// ------------- others --------------//

	// path
	QString directory;

	// flag
	int animationShown = 0;
	int moveFigureShown = 0;
	// move joints
	double xPrevious = 90.0f, yPrevious = 90.0f, zPrevious = 90.0f;

// constructor
	MyMainWindow(QWidget* parent = nullptr);

protected:
// variables
	double previousCoord[2];
	double shiftAmountX, shiftAmountY, shiftAmountZ;
	double rotateViewAngle;

// functions
	bool event(QEvent* e) override;
	
// slots
public slots:
	void selectFileDialog();
	void createAnimationOn();
	void checkDamped();
	void createNewFrame();
	void createNewFile();
};
