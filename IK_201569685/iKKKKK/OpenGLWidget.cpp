#include "OpenGLWidget.h"


OpenGLWidget::OpenGLWidget(QWidget* parent):QOpenGLWidget(parent)
{
	connect(timer, &QTimer::timeout,this,&OpenGLWidget::checkTime);
}

void OpenGLWidget::playAnimation(int flag) {
	animationOn = flag;
}

void OpenGLWidget::initializeGL(){
	f = this->context()->functions();
    // run and link shader program
    m_program = new QOpenGLShaderProgram(this);

    // there are two shader. one vertex shader and one fragment shader.
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);

    // link the two shader
    if (!m_program->link()) {
		return;
    }
   
    

    // create link to the attributes and uniform of the shaders
    m_posAttr = m_program->attributeLocation("posAttr");
    Q_ASSERT(m_posAttr != -1);
    m_colAttr = m_program->attributeLocation("colAttr");
    Q_ASSERT(m_colAttr != -1);
    m_matrixUniform = m_program->uniformLocation("matrix");
    Q_ASSERT(m_matrixUniform != -1);

	
    // initial the background
    
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    f->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

}

void OpenGLWidget::resizeGL(int w, int h) {

    const qreal retinaScale = devicePixelRatio();
    f->glViewport(0, 0, w * retinaScale,h * retinaScale);
    f->glClearColor(0.0f,0.0f,0.0f,1.0f);

}

void OpenGLWidget::paintGL(){
	
	// set up timer to count for next frame
	if (nextFrameOn == 0)
	{
		timer->start(1000 * frameTime);
		nextFrameOn = 1;
	}

    // bind shader
    m_program->bind();

	// draw the shapes and movements
	render();

    // release the shader
    m_program->release();

    // frame move forward 1 step
	drawNextFrame();
	


}

void OpenGLWidget::drawNextFrame() {
	
	
	if (animationOn == 1 && nextFrameOn == 2) 
	// if is in animation mode and next frame is ready to play
	// do the following
	{
		
		// reset the nextFrameOn flag
		nextFrameOn = 0;

		// move to next frame
		frameNum += 1;
		
	}
	this->update();
}

void OpenGLWidget::setFrame(int _frameNum) {
	frameNum = _frameNum;
}

int OpenGLWidget::checkFrameNum() {
	return frameNum;
}

void OpenGLWidget::drawCylinder(float radius, float length, float x, float y, float z) {

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


		f->glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, vertices);
		f->glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, colors);

		f->glEnableVertexAttribArray(m_posAttr);
		f->glEnableVertexAttribArray(m_colAttr);

		// triangle primitive
		f->glDrawArrays(GL_TRIANGLES, 0, 3);

		f->glDisableVertexAttribArray(m_colAttr);
		f->glDisableVertexAttribArray(m_posAttr);

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


		f->glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, vertices);
		f->glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, colors);

		f->glEnableVertexAttribArray(m_posAttr);
		f->glEnableVertexAttribArray(m_colAttr);

		// triangle primitive
		f->glDrawArrays(GL_TRIANGLES, 0, 3);

		f->glDisableVertexAttribArray(m_colAttr);
		f->glDisableVertexAttribArray(m_posAttr);

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


		f->glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, vertices);
		f->glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, colors);

		f->glEnableVertexAttribArray(m_posAttr);
		f->glEnableVertexAttribArray(m_colAttr);

		// triangle primitive
		f->glDrawArrays(GL_TRIANGLES, 0, 3);

		f->glDisableVertexAttribArray(m_colAttr);
		f->glDisableVertexAttribArray(m_posAttr);

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


		f->glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, vertices);
		f->glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, colors);

		f->glEnableVertexAttribArray(m_posAttr);
		f->glEnableVertexAttribArray(m_colAttr);

		// triangle primitive
		f->glDrawArrays(GL_TRIANGLES, 0, 3);

		f->glDisableVertexAttribArray(m_colAttr);
		f->glDisableVertexAttribArray(m_posAttr);

	}

}



void OpenGLWidget::render() {
	
}

void OpenGLWidget::checkTime()
{
	nextFrameOn = 2;
}