#include "ReadbvhFile.h"

ReadbvhFile::ReadbvhFile(const char* fileName, QWidget* parent, float _scale):OpenGLWidget(parent),scale(_scale) {
	// read bvh file
	loadBVHFile(fileName);
	playAnimation(1);
	
	
}
ReadbvhFile::ReadbvhFile( QWidget* parent, float _scale) :OpenGLWidget(parent), scale(_scale) {
	// read bvh file
	playAnimation(1);


}

void ReadbvhFile::cleanUp()
{
	// read bvh file
	playAnimation(1);
	frameNum = 0;
}

void ReadbvhFile::render() {
	
	// initial matrix
	if(matrix.size()>0)
		matrix.clear();

	QMatrix4x4 temp;
	

	matrix.push_back(QMatrix4x4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f));
	//matrix.back().perspective(120.0f, 4.0f / 3.0f, 1.0f, 100.0f);
	
	matrix.back().ortho( - 50.5 * width() / height(), 50.5 * width() / height(), -50.5, 50.5, -150, 150);;
	
	matrix.back().rotate(rotateAngle,0,1,0);

	RenderFigure(frameNum, scale);


	// check if the frame number for the animation reaches the maximum number of total frames
	if (checkFrameNum() >= num_frame) {
		// if so, turn frameNum back to 0
		setFrame(-1);
	}
	

}


void  ReadbvhFile::RenderBone(float x0, float y0, float z0, float x1, float y1, float z1, float bRadius)
{
	// draw cylinder to connect the two vertices

	// get info about the cylinder
	GLdouble  dir_x = x1 - x0;
	GLdouble  dir_y = y1 - y0;
	GLdouble  dir_z = z1 - z0;
	GLdouble  bone_length = sqrt(dir_x * dir_x + dir_y * dir_y + dir_z * dir_z);


	// translate position
	QMatrix4x4 transformMatrix;

	transformMatrix = matrix.back();

	matrix.push_back(transformMatrix);

	// translate the cylinder
	matrix.back().translate(x0, y0, z0);

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
	GLdouble angle = acos(cosTheta) / 2;
	cosTheta = cos(angle);
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
		float(1 - 2 * (rotateAxis_y * rotateAxis_y + rotateAxis_z * rotateAxis_z)), float(2 * (rotateAxis_x * rotateAxis_y - cosTheta * rotateAxis_z)), float(2 * (rotateAxis_x * rotateAxis_z + cosTheta * rotateAxis_y)), 0.0f,
		float(2 * (rotateAxis_x * rotateAxis_y + cosTheta * rotateAxis_z)), float(1 - 2 * (rotateAxis_x * rotateAxis_x + rotateAxis_z * rotateAxis_z)), float(2 * (rotateAxis_y * rotateAxis_z - rotateAxis_x * cosTheta)), 0.0f,
		float(2 * (rotateAxis_x * rotateAxis_z - cosTheta * rotateAxis_y)), float(2 * (rotateAxis_x * cosTheta + rotateAxis_y * rotateAxis_z)), float(1 - 2 * (rotateAxis_x * rotateAxis_x + rotateAxis_y * rotateAxis_y)), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f

	);


	matrix.back() = matrix.back() * quaternion;

	m_program->setUniformValue(m_matrixUniform, matrix.back());

	// ƒÒÖù¤ÎÔO¶¨
	GLdouble radius = bRadius; // ƒÒÖù¤ÎÌ«¤µ

	// draw cylinder
	drawCylinder(radius, bone_length, 0.0f, 0.0f, 0.0f);
	

	if(matrix.size()>0)
		matrix.pop_back();

}

// Draw the posture of the specified frame
void ReadbvhFile::RenderFigure(int frame_no, float scale)
{
	// BVH skeleton / posture specified for drawing
	if (motion == nullptr) {

		return;
	}
	if(joints.size()>0)
		RenderFigure(joints[0], motion + (frame_no * num_channel), scale);
	
}


void ReadbvhFile::RenderFigure(const Joint* joint, const double* data, float scale)
{

	// set transformation matrix
	QMatrix4x4 tempMatrix, translate, rotate;

	
	tempMatrix = matrix.back();
	

	matrix.push_back(tempMatrix);


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
		if (channel->type == X_ROTATION) {
			rotate.rotate(data[channel->index], 1.0f, 0.0f, 0.0f);

		}
		else if (channel->type == Y_ROTATION) {
			rotate.rotate(data[channel->index], 0.0f, 1.0f, 0.0f);

		}
		else if (channel->type == Z_ROTATION) {
			rotate.rotate(data[channel->index], 0.0f, 0.0f, 1.0f);
		}
	}

	

	// do the transformation
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
			RenderBone(center[0] * scale, center[1] * scale, center[2] * scale, child->offset[0] * scale, child->offset[1] * scale, child->offset[2] * scale);
		
		}

		

	}


	// Recursive call to the child joint
	for (i = 0; i < joint->children.size(); i++)
	{
		RenderFigure(joint->children[i], data, scale);
	}

	if(matrix.size()>0)
		matrix.pop_back();

}



void ReadbvhFile::loadBVHFile(const char* fileName) {
	Load(fileName);
	frameTime = interval;
}


