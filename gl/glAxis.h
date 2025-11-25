#pragma once
#include "glInclude.h"


class glAxis : protected QOpenGLFunctions
{
	public:
	glAxis();
	~glAxis();
	void init() { initializeOpenGLFunctions(); }
	void draw(void);
	void shaderProgramInit();

	void initAxis();

	void setmvpMatrix(const QMatrix4x4& mvp)
	{
		m_mvp = mvp;
		shaderProgram->bind();
		shaderProgram->setUniformValue("mvpMatrix", mvp);
		shaderProgram->release();
	}
	void setMVP(QMatrix4x4 model, QMatrix4x4 view, QMatrix4x4 projection);


	QMatrix4x4 m_mvp;
	QOpenGLShaderProgram* shaderProgram = nullptr;
	QOpenGLVertexArrayObject* axisVAO = nullptr;
	QOpenGLBuffer* axisVBO = nullptr;
	GLuint axisVertexCount = 0;
};

