#pragma once

#include <fstream>
#include <QVector4D>
#include <QMatrix4x4>
#include "ReadbvhFile.h"
#include "stdafx.h"

#include <qDebug>
#include <string>
#include <sstream>
#include <iostream>

// Eigen library
#include <Core>
#include <SVD>
#include <Dense>


class MoveFigure: public ReadbvhFile{

public:
	
	// parameter
	std::fstream BVHFile;
	std::fstream frameData;
	
	int frameNumForNewFile;

	int endEffectorID;

	double targetPos[4];
	

	// world position of joints in jointsLink
	// calculate by FK
	// the first one is end effector
	std::vector<Joint*> jointsLink;


	std::vector<float> jacobian;
	bool useLambda;
	int lambda;


	QVector4D firstJoint;
	
	Joint* root;

	double* data;
	

	QVector4D endEffectorPos, EEScreenPos;
	QVector4D goalPos;
	
	int clickedJoint;
	
	int axisID[3] = { Z_ROTATION, Y_ROTATION, X_ROTATION };
	

	// constructor
	MoveFigure(QWidget* parent = nullptr, float _scale = 2 );
	
	// functions
	void cleanUp();

	void openRestFile();

	void createNewFile();

	void setEndEffectorID(int EID);

	void getJointsLink(int EID);

	void setFrame(int _frameNum);

	void setScale(double _scale);

	void ClickJoint(float _x,float _y);
	
	void setGoalPos(double x, double y);
	
	void Jacobian(int EID);

	void CalculateAngle( int _frameNum);
	
	
	Eigen::MatrixXf PseudoInverse(float* originalMatrix);

	QMatrix4x4 ForwardKinematics( int EEID, int _jointID_Rotate = -1, double _rotateStep = 0, int Axis =-1);

	QMatrix4x4 ForwardKinematicsGeneral(int endID = -1);

};