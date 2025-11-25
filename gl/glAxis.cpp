#include "glAxis.h"

glAxis::glAxis()
{

}

glAxis::~glAxis()
{

}

void glAxis::draw(void)
{
	shaderProgram->bind();
	// 绘制坐标轴
	glLineWidth(5.0f);
	axisVAO->bind();
	glDrawArrays(GL_LINES, 0, axisVertexCount);
	axisVAO->release();
	shaderProgram->release();
}

void glAxis::shaderProgramInit()
{
	shaderProgram = new QOpenGLShaderProgram();
	shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex,
		"#version 330 core\n"
		"layout(location = 0) in vec3 position;\n"
		"layout(location = 1) in vec3 color;\n"
		"uniform mat4 mvpMatrix;\n"
		"out vec3 fragColor;\n"
		"void main() {\n"
		"    gl_Position = mvpMatrix * vec4(position, 1.0);\n"
		"    fragColor = color;\n"
		"}");
	shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment,
		"#version 330 core\n"
		"in vec3 fragColor;\n"
		"out vec4 finalColor;\n"
		"void main() {\n"
		"    finalColor = vec4(fragColor, 1.0);\n"
		"}");
bool linkRet=	shaderProgram->link();
	if (!linkRet) {
		qDebug() << "Shader program linking failed:" << shaderProgram->log();
		return;
	}
	
}

void glAxis::initAxis()
{
	// 创建坐标轴顶点数据
	QVector<float> vertices;

	// X轴 (红色)
	vertices << 0.0f << 0.0f << 0.0f << 1.0f << 0.0f << 0.0f;
	vertices << 2.0f << 0.0f << 0.0f << 1.0f << 0.0f << 0.0f;

	// Y轴 (绿色)
	vertices << 0.0f << 0.0f << 0.0f << 0.0f << 1.0f << 0.0f;
	vertices << 0.0f << 2.0f << 0.0f << 0.0f << 1.0f << 0.0f;

	// Z轴 (蓝色)
	vertices << 0.0f << 0.0f << 0.0f << 0.0f << 0.0f << 1.0f;
	vertices << 0.0f << 0.0f << 2.0f << 0.0f << 0.0f << 1.0f;

	axisVertexCount = vertices.size() / 6; // 每个顶点有6个值

	// 创建VAO和VBO
	axisVAO = new QOpenGLVertexArrayObject();
	axisVAO->create();
	axisVAO->bind();

	axisVBO = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
	axisVBO->create();
	axisVBO->bind();
	axisVBO->allocate(vertices.constData(), vertices.size() * sizeof(float));

	// 设置顶点属性指针
	shaderProgram->enableAttributeArray(0);
	shaderProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, 6 * sizeof(float));

	shaderProgram->enableAttributeArray(1);
	shaderProgram->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(float), 3, 6 * sizeof(float));

	axisVAO->release();
}

void glAxis::setMVP(QMatrix4x4 model, QMatrix4x4 view, QMatrix4x4 projection)
{
	//QMatrix4x4 mvp = projection * view * model;
	QMatrix4x4 mvp = projection * view ;

	m_mvp = mvp;

	shaderProgram->bind();
	shaderProgram->setUniformValue("mvpMatrix", mvp);
	shaderProgram->release();
}
