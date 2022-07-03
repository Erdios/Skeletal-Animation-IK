#pragma once

#include "BVH.h"
#include "QtGUI.h"
#include "stdafx.h"

class ReadBVH : public BVH, public OpenGLWidget
{

public:
	// set transformation matrix
	std::vector<QMatrix4x4> matrix;
	
	int frameNum;

	ReadBVH();
	void RenderBone(float x0, float y0, float z0, float x1, float y1, float z1, float bRadius = 0.1);
	void render() override;
	void RenderFigure(const Joint* joint, const double* data, float scale);
	void RenderFigure(int frame_no, float scale);
	void drawCylinder(float radius, float length, float x, float y, float z);
	void drawSphere(float radius, float x, float y, float z);

};


