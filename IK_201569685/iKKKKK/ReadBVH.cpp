#include "ReadBVH.h"


ReadBVH::ReadBVH(){
	
	frameNum = 0;
	Load("02_01.bvh");

}

void ReadBVH::render() {

	// get pixel ratio
	const qreal retinaScale = devicePixelRatio();
	
	
	// initial viewport
	// glViewport(0, 0, width() * retinaScale, height() * retinaScale);
	//glClear(GL_COLOR_BUFFER_BIT);
	matrix.clear();

	// bind shader
	m_program->bind();

	
	matrix.push_back(QMatrix4x4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f));
	//matrix.back().perspective(120.0f, 4.0f / 3.0f, 1.0f, 100.0f);
	matrix.back().ortho(-50.5,50.5,-50.5,50.5,-150,150);
	RenderFigure(frameNum,1);

	// release the shader
	m_program->release();

	// check if the frame number for the animation reaches the maximum number of total frames
	if (frameNum >= num_frame) {

		// if so, turn frameNum back to 0
		frameNum = 0;
	}
	else {
		// if not, increase frameNum by 1
		frameNum = frameNum + 1;
	}

	
}

void  ReadBVH::RenderBone(float x0, float y0, float z0, float x1, float y1, float z1, float bRadius)
{
	// draw cylinder to connect the two vertices

	// get info about the cylinder
	GLdouble  dir_x = x1 - x0;
	GLdouble  dir_y = y1 - y0;
	GLdouble  dir_z = z1 - z0;
	GLdouble  bone_length = sqrt(dir_x * dir_x + dir_y * dir_y + dir_z * dir_z);


	// translate position
	QMatrix4x4 transformMatrix,translate;
	
	if (matrix.size() > 0) {
		transformMatrix = matrix.back();
		
	}
	
	matrix.push_back(transformMatrix);

	translate.translate(x0, y0, z0);
	// calculate rotation matrix

	// Normalize z-axis to unit vector
	GLdouble lengthDir = bone_length;
	
	

	// Set the orientation of the reference y-axis
	GLdouble  cylinderDir_x, cylinderDir_y, cylinderDir_z;
	cylinderDir_x = 0.0;
	cylinderDir_y = 0.0;
	cylinderDir_z = 1.0;


	// dot product get cos theta because norm of dir and cylinderDir are 1
	GLdouble cosTheta = (dir_x * cylinderDir_x + dir_y * cylinderDir_y + dir_z * cylinderDir_z) / lengthDir;
	GLdouble angle = acos(cosTheta)/2;
	cosTheta =  cos(angle);
	GLdouble sinTheta = sin(angle);

	// calculate the rotation axis for rotating the dir to the direction of cylinder
	if (lengthDir < 0.0001) {
		dir_x = 0.0; dir_y = 0.0; dir_z = 1.0;  lengthDir = 1.0;
	}
	dir_x /= lengthDir;  dir_y /= lengthDir;  dir_z /= lengthDir;
	GLdouble  rotateAxis_x, rotateAxis_y, rotateAxis_z;
	rotateAxis_x = cylinderDir_y * dir_z - cylinderDir_z * dir_y;
	rotateAxis_y = cylinderDir_z * dir_x - cylinderDir_x * dir_z;
	rotateAxis_z = cylinderDir_x * dir_y - cylinderDir_y * dir_x;


	// Normalize the rotationAxis
	GLdouble lengthRotate = sqrt(rotateAxis_x * rotateAxis_x + rotateAxis_y * rotateAxis_y + rotateAxis_z * rotateAxis_z);
	if (lengthRotate < 0.0001) {
		rotateAxis_x = 1.0; rotateAxis_y = 0.0; rotateAxis_z = 0.0;  lengthRotate = 1.0;
	}
	//rotateAxis_x /= lengthRotate;  rotateAxis_y /= lengthRotate;  rotateAxis_z /= lengthRotate;

	rotateAxis_x = rotateAxis_x * sinTheta;
	rotateAxis_y = rotateAxis_y * sinTheta;
	rotateAxis_z = rotateAxis_z * sinTheta;

	// get quaternion
	QMatrix4x4 quaternion(
		float(1 - 2 * (rotateAxis_y * rotateAxis_y + rotateAxis_z * rotateAxis_z)),float(2 * (rotateAxis_x * rotateAxis_y - cosTheta * rotateAxis_z)), float(2 * (rotateAxis_x * rotateAxis_z + cosTheta * rotateAxis_y)), 0.0f,
		float(2*(rotateAxis_x*rotateAxis_y+cosTheta*rotateAxis_z)), float(1-2*(rotateAxis_x*rotateAxis_x + rotateAxis_z*rotateAxis_z)), float(2*(rotateAxis_y*rotateAxis_z-rotateAxis_x*cosTheta)),0.0f,
		float(2*(rotateAxis_x*rotateAxis_z-cosTheta*rotateAxis_y)), float(2*(rotateAxis_x*cosTheta +rotateAxis_y*rotateAxis_z)), float(1-2*(rotateAxis_x*rotateAxis_x+rotateAxis_y*rotateAxis_y)),0.0f,
		0.0f,0.0f,0.0f,1.0f
	
	);


	matrix.back() = matrix.back() * translate*quaternion;
	
	//QMatrix4x4 result;


	//for (int r = 0; r < 4;r++) {
	//	for (int c = 0; c < 4; c++) {
	//			
	//		result(r, c) = matrix.back()(r,0) * m[0*4+c] + matrix.back()(r, 1) * m[1 * 4 + c] + matrix.back()(r, 2) * m[2 * 4 + c] + matrix.back()(r, 3) * m[3 * 4 + c];
	//	
	//	}
	//
	//
	//}
	//matrix.back() = result;

	m_program->setUniformValue(m_matrixUniform,  matrix.back());
	
	// 円柱の設定
	GLdouble radius = bRadius; // 円柱の太さ

	// draw cylinder
	
	drawCylinder(radius, bone_length, 0.0f, 0.0f,0.0f);
	

	matrix.pop_back();

}

// Draw the posture of the specified frame
void  ReadBVH::RenderFigure(int frame_no, float scale)
{
	// BVH skeleton / posture specified for drawing
	RenderFigure(joints[0], motion + (frame_no * num_channel), scale);
}

 //draw figure
void  ReadBVH::RenderFigure(const Joint* joint, const double* data, float scale)
{

	// set transformation matrix
	QMatrix4x4 transformMatrix, translate, rotate;

	if (matrix.size() > 0) {
		transformMatrix = matrix.back();
	}

	matrix.push_back(transformMatrix);
	
	
	// Apply translation for root joints
	if (joint->parent == NULL)
	{
		translate.translate(data[0] * scale, data[1] * scale, data[2] * scale); 
	}

	// For child joints, apply translation from the parent joint
	else
	{
		translate.translate(joint->offset[0] * scale, joint->offset[1] * scale, joint->offset[2] * scale); 
	}
	
	// Apply rotation from parent joint(rotation from world coordinates for root joint)
	int  i;
	for (i = 0; i < joint->channels.size(); i++)
	{
		Channel* channel = joint->channels[i];
		if (channel->type == X_ROTATION){
			rotate.rotate(data[channel->index], 1.0f, 0.0f, 0.0f);

		}
		else if (channel->type == Y_ROTATION){
			rotate.rotate(data[channel->index], 0.0f, 1.0f, 0.0f);

		}
		else if (channel->type == Z_ROTATION) {
			rotate.rotate(data[channel->index], 0.0f, 0.0f, 1.0f);
		}
	}

	matrix.back() = matrix.back() * translate * rotate;
	m_program->setUniformValue(m_matrixUniform, matrix.back());

	// Draw a link
	// Draw a link from the origin to the end of the joint coordinate system
	if (joint->children.size() == 0)
	{
		RenderBone(0.0f, 0.0f, 0.0f, joint->site[0] * scale, joint->site[1] * scale, joint->site[2] * scale);
	}

	// Draw a link from the origin of the joint coordinate system to the connection position to the next joint
	if (joint->children.size() == 1)
	{
		Joint* child = joint->children[0];
		RenderBone(0.0f, 0.0f, 0.0f, child->offset[0] * scale, child->offset[1] * scale, child->offset[2] * scale);
	}
	// Draw a cylinder from the center point to the connection position to all joints to the connection position to each joint
	if (joint->children.size() > 1)
	{

		// Calculate the center point to the origin and the connection position to all joints
		float  center[3] = { 0.0f, 0.0f, 0.0f };
		for (i = 0; i < joint->children.size(); i++)
		{
			Joint* child = joint->children[i];
			center[0] += child->offset[0];
			center[1] += child->offset[1];
			center[2] += child->offset[2];
		}
		center[0] /= joint->children.size() + 1;
		center[1] /= joint->children.size() + 1;
		center[2] /= joint->children.size() + 1;


		// Draw a link from the origin to the center point
		RenderBone(0.0f, 0.0f, 0.0f, center[0] * scale, center[1] * scale, center[2] * scale);

		// Draw a link from the center point to the connection position to the next joint
		for (i = 0; i < joint->children.size(); i++)
		{
			Joint* child = joint->children[i];
			RenderBone(center[0] * scale, center[1] * scale, center[2] * scale,child->offset[0] * scale, child->offset[1] * scale, child->offset[2] * scale);
		}
	}


	// Recursive call to the child joint
	for (i = 0; i < joint->children.size(); i++)
	{
		RenderFigure(joint->children[i], data, scale);
	}

	matrix.pop_back();
	
}


void ReadBVH::drawCylinder(float radius, float length, float x, float y, float z) {

	GLfloat vertices[] = {
		 0.0f, 0.0f, 0.0f,
		 0.0f, 0.0f, 0.0f,
		 0.0f, 0.0f, 0.0f,

	};
	GLfloat colors[] = {
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f
	};

	double a1, a2;

	colors[0] = 0.0f;
	colors[1] = 1.0f;
	colors[3] = 0.0f;
	colors[4] = 1.0f;
	colors[6] = 0.0f;
	colors[7] = 1.0f;

	for (int i = 0; i <= 360; i = i + 5) {

		a1 = i * 3.14 / 180;
		a2 = (i + 5) * 3.14 / 180;


		vertices[0] = x + radius * sin(a1);
		vertices[1] = y + radius * cos(a1);
		vertices[2] = z;

		vertices[3] = x + radius * sin(a2);
		vertices[4] = y + radius * cos(a2);
		vertices[5] = z;

		vertices[6] = x + 0.0f;
		vertices[7] = y + 0.0f;
		vertices[8] = z;


		glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, vertices);
		glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, colors);

		glEnableVertexAttribArray(m_posAttr);
		glEnableVertexAttribArray(m_colAttr);

		// triangle primitive
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glDisableVertexAttribArray(m_colAttr);
		glDisableVertexAttribArray(m_posAttr);

	}

	colors[0] = 1.0f;
	colors[1] = 0.0f;
	colors[3] = 1.0f;
	colors[4] = 0.0f;
	colors[6] = 1.0f;
	colors[7] = 0.0f;
	for (int i = 0; i <= 360; i = i + 5) {

		a1 = i * 3.14 / 180;
		a2 = (i + 5) * 3.14 / 180;


		vertices[0] = x + radius * sin(a1);
		vertices[1] = y + radius * cos(a1);
		vertices[2] = z + length;

		vertices[3] = x + radius * sin(a2);
		vertices[4] = y + radius * cos(a2);
		vertices[5] = z + length;

		vertices[6] = x + radius * sin(a1);
		vertices[7] = y + radius * cos(a1);
		vertices[8] = z;


		glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, vertices);
		glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, colors);

		glEnableVertexAttribArray(m_posAttr);
		glEnableVertexAttribArray(m_colAttr);

		// triangle primitive
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glDisableVertexAttribArray(m_colAttr);
		glDisableVertexAttribArray(m_posAttr);

	}
	for (int i = 0; i <= 360; i = i + 5) {

		a1 = i * 3.14 / 180;
		a2 = (i + 5) * 3.14 / 180;


		vertices[0] = x + radius * sin(a1);
		vertices[1] = y + radius * cos(a1);
		vertices[2] = z;

		vertices[3] = x + radius * sin(a2);
		vertices[4] = y + radius * cos(a2);
		vertices[5] = z;

		vertices[6] = x + radius * sin(a2);
		vertices[7] = y + radius * cos(a2);
		vertices[8] = z + length;


		glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, vertices);
		glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, colors);

		glEnableVertexAttribArray(m_posAttr);
		glEnableVertexAttribArray(m_colAttr);

		// triangle primitive
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glDisableVertexAttribArray(m_colAttr);
		glDisableVertexAttribArray(m_posAttr);

	}
	colors[0] = 0.0f;
	colors[1] = 1.0f;
	colors[3] = 0.0f;
	colors[4] = 1.0f;
	colors[6] = 0.0f;
	colors[7] = 1.0f;
	for (int i = 0; i <= 360; i = i + 5) {

		a1 = i * 3.14 / 180;
		a2 = (i + 5) * 3.14 / 180;


		vertices[0] = x + radius * sin(a1);
		vertices[1] = y + radius * cos(a1);
		vertices[2] = z + length;

		vertices[3] = x + radius * sin(a2);
		vertices[4] = y + radius * cos(a2);
		vertices[5] = z + length;

		vertices[6] = x + 0.0f;
		vertices[7] = y + 0.0f;
		vertices[8] = z + length;


		glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, vertices);
		glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, colors);

		glEnableVertexAttribArray(m_posAttr);
		glEnableVertexAttribArray(m_colAttr);

		// triangle primitive
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glDisableVertexAttribArray(m_colAttr);
		glDisableVertexAttribArray(m_posAttr);

	}

}


void ReadBVH::drawSphere(float radius, float x, float y, float z) {

	GLfloat vertices[] = {
		 0.0f, 0.0f, 0.0f,
		 0.0f, 0.0f, 0.0f,
		 0.0f, 0.0f, 0.0f

	};
	static const GLfloat colors[] = {
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f
	};

	float stepz = 10 * 3.14 / 180, stepxy = 10 * 3.14 / 180;
	int M = 36, N = 36;
	double axy = 0, az = 0;

	for (int i = 0; i <= M; i = i + 1) {


		for (int j = 0; j <= N; j = j + 1) {


			vertices[0] = x + radius * sin(az) * cos(axy); //x
			vertices[1] = y + radius * sin(az) * sin(axy); //y
			vertices[2] = z + radius * cos(az); //z

			vertices[3] = x + radius * sin(az + stepz) * cos(axy); //x
			vertices[4] = y + radius * sin(az + stepz) * sin(axy); //y
			vertices[5] = z + radius * cos(az + stepz); //z

			vertices[6] = x + radius * sin(az + stepz) * cos(axy + stepxy); //x
			vertices[7] = y + radius * sin(az + stepz) * sin(axy + stepxy); //y
			vertices[8] = z + radius * cos(az + stepz); //z



			glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, vertices);
			glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, colors);

			glEnableVertexAttribArray(m_posAttr);
			glEnableVertexAttribArray(m_colAttr);

			glDrawArrays(GL_TRIANGLES, 0, 3);

			glDisableVertexAttribArray(m_colAttr);
			glDisableVertexAttribArray(m_posAttr);


			vertices[0] = x + radius * sin(az) * cos(axy + stepxy); //x
			vertices[1] = y + radius * sin(az) * sin(axy + stepxy); //y
			vertices[2] = z + radius * cos(az); //z

			vertices[3] = x + radius * sin(az + stepz) * cos(axy + stepxy); //x
			vertices[4] = y + radius * sin(az + stepz) * sin(axy + stepxy); //y
			vertices[5] = z + radius * cos(az + stepz); //z

			vertices[6] = x + radius * sin(az) * cos(axy); //x
			vertices[7] = y + radius * sin(az) * sin(axy); //y
			vertices[8] = z + radius * cos(az); //z

			glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, vertices);
			glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, colors);

			glEnableVertexAttribArray(m_posAttr);
			glEnableVertexAttribArray(m_colAttr);

			glDrawArrays(GL_TRIANGLES, 0, 3);

			glDisableVertexAttribArray(m_colAttr);
			glDisableVertexAttribArray(m_posAttr);

			axy += stepxy;
		}

		az += stepz;

	}

}