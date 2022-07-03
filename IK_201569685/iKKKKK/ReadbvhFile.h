#pragma once
#include "stdafx.h"
#include "OpenGLWidget.h"
#include "BVH.h"


class ReadbvhFile : public OpenGLWidget, public BVH {

public:
	// parameter
	std::vector<QMatrix4x4> matrix;
	double rotateAngle = 0;
	
	

	// figure scale
	float scale;
	

	// constructor
	ReadbvhFile(const char* fileName, QWidget* parent = nullptr, float _scale = 2);
	ReadbvhFile(QWidget* parent = nullptr, float _scale = 2);
	
	
	// function
	void cleanUp();
	void render() override;
	void RenderBone(float x0, float y0, float z0, float x1, float y1, float z1, float bRadius = 0.1);
	void RenderFigure(int frame_no, float scale);
	void RenderFigure(const Joint* joint, const double* data, float scale);
	void loadBVHFile(const char* fileName);
	

};