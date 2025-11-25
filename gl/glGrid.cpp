#include "glGrid.h"

glGrid::glGrid()
{

}

glGrid::~glGrid()
{

}

void glGrid::draw(void)
{
	shaderProgram->bind();

	// 绘制网格
	glLineWidth(1.0f);
	gridVAO->bind();
	glDrawArrays(GL_LINES, 0, gridVertexCount);
	gridVAO->release();


	shaderProgram->release();
}

void glGrid::shaderProgramInit()
{
	// 创建着色器程序
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
	shaderProgram->link();
}

void glGrid::initGrid()
{
	// 创建网格顶点数据
	QVector<float> vertices;
	const int size = 20;
	const float step = 1.0f;

	// 网格颜色 (灰色)
	QVector3D gridColor(0.5f, 0.5f, 0.5f);

	for (int i = -size; i <= size; ++i)
	{
		// 水平线 (X方向)
		vertices << i * step << 0 << -size * step;
		vertices << gridColor.x() << gridColor.y() << gridColor.z();
		vertices << i * step << 0 << size * step;
		vertices << gridColor.x() << gridColor.y() << gridColor.z();

		// 垂直线 (Z方向)
		vertices << -size * step << 0 << i * step;
		vertices << gridColor.x() << gridColor.y() << gridColor.z();
		vertices << size * step << 0 << i * step;
		vertices << gridColor.x() << gridColor.y() << gridColor.z();
	}

	gridVertexCount = vertices.size() / 6; // 每个顶点有6个值 (3位置 + 3颜色)

	// 创建VAO和VBO
	gridVAO = new QOpenGLVertexArrayObject();
	gridVAO->create();
	gridVAO->bind();

	gridVBO = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
	gridVBO->create();
	gridVBO->bind();
	gridVBO->allocate(vertices.constData(), vertices.size() * sizeof(float));

	// 设置顶点属性指针
	shaderProgram->enableAttributeArray(0);
	shaderProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, 6 * sizeof(float));

	shaderProgram->enableAttributeArray(1);
	shaderProgram->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(float), 3, 6 * sizeof(float));

	gridVAO->release();
}

void glGrid::setMVP(QMatrix4x4 model, QMatrix4x4 view, QMatrix4x4 projection)
{
	//QMatrix4x4 mvp = projection * view * model;
	QMatrix4x4 mvp = projection * view ;
	m_mvp = mvp;

	shaderProgram->bind();
	shaderProgram->setUniformValue("mvpMatrix", mvp);
	shaderProgram->release();
}
