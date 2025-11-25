#pragma once
#include "glInclude.h"


class glGrid : protected QOpenGLFunctions
{
public:
	glGrid();
	~glGrid();
	void init() { initializeOpenGLFunctions(); }
	void draw(void);
	void shaderProgramInit();

	void initGrid();

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
	QOpenGLVertexArrayObject* gridVAO = nullptr;
	QOpenGLBuffer* gridVBO = nullptr;
	GLuint gridVertexCount = 0;
};

