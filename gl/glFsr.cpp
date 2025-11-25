#include "glFsr.h"

glFsr::glFsr()
{
}

glFsr::~glFsr()
{

}

void glFsr::addXYAxisRect(int i, int y)
{
	float scale = 0.25;
	float xOffset = -5.0f / 10;
	float yOffset = -10.0f / 10;
	float zOffset = 3.1f / 10;
	//立方体的8个顶点
	GLfloat vertices[] = {
		0.0f + i, 0.0f + y, 0.0f,
		1.0f + i, 0.0f + y, 0.0f,
		1.0f + i, 1.0f + y, 0.0f,
		0.0f + i, 1.0f + y, 0.0f,
	};

	for (int i = 0; i < sizeof(vertices) / sizeof(vertices[0]); i++)
	{
		vertices[i] *= scale;
	}

	vertices[0] += xOffset;
	vertices[1] += yOffset;
	vertices[2] += zOffset;
	vertices[3] += xOffset;
	vertices[4] += yOffset;
	vertices[5] += zOffset;
	vertices[6] += xOffset;
	vertices[7] += yOffset;
	vertices[8] += zOffset;
	vertices[9] += xOffset;
	vertices[10] += yOffset;
	vertices[11] += zOffset;

	//默认顶点颜色
	GLfloat color[] = {
		0.2f,
		0.2f,
		0.2f
	};

	GLushort indices[] = {
		// 前面
		0, 1, 2, 2, 3, 0,
	};

	for (int i = 0; i < 1; i++)
	{
		PosColorVec6 vec6;
		vec6.r = color[0];
		vec6.g = color[1];
		vec6.b = color[2];

		vec6.x = vertices[0 * 3 + 0];
		vec6.y = vertices[0 * 3 + 1];
		vec6.z = vertices[0 * 3 + 2];
		vertexData.append(vec6);

		vec6.x = vertices[1 * 3 + 0];
		vec6.y = vertices[1 * 3 + 1];
		vec6.z = vertices[1 * 3 + 2];
		vertexData.append(vec6);

		vec6.x = vertices[2 * 3 + 0];
		vec6.y = vertices[2 * 3 + 1];
		vec6.z = vertices[2 * 3 + 2];
		vertexData.append(vec6);

		vec6.x = vertices[2 * 3 + 0];
		vec6.y = vertices[2 * 3 + 1];
		vec6.z = vertices[2 * 3 + 2];
		vertexData.append(vec6);

		vec6.x = vertices[3 * 3 + 0];
		vec6.y = vertices[3 * 3 + 1];
		vec6.z = vertices[3 * 3 + 2];
		vertexData.append(vec6);

		vec6.x = vertices[0 * 3 + 0];
		vec6.y = vertices[0 * 3 + 1];
		vec6.z = vertices[0 * 3 + 2];
		vertexData.append(vec6);
	}
}

void glFsr::setupGeometry(void)
{
	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 8; y++)
		{
			addXYAxisRect(x, y);
		}
	}

	//设置立方体的VA0 VB0
	vao = new QOpenGLVertexArrayObject();
	vao->create();
	vao->bind();

	vbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
	vbo->create();
	vbo->bind();
	vbo->allocate((const void*)&(vertexData[0]), vertexData.size() * sizeof(PosColorVec6));

	program->bind();
	program->enableAttributeArray(0); // 位置属性位置0
	program->setAttributeBuffer(0, GL_FLOAT, 0, 3, 2 * sizeof(QVector3D)); // 每个顶点3个float，位置属性偏移0
	program->enableAttributeArray(1); // 颜色属性位置1
	program->setAttributeBuffer(1, GL_FLOAT, sizeof(QVector3D), 3, 2 * sizeof(QVector3D)); // 每个顶点3个float，颜色属性偏移3个float
	program->release();

	vbo->release();
	vao->release();
}

void glFsr::shaderProgramInit(void)
{
	program = new QOpenGLShaderProgram();
	program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
	program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
	program->link();

}

void glFsr::draw(void)
{
	program->bind();
	vao->bind();
	vbo->bind();

	//根据压力片更改颜色
#if 1
	{
		for (int x = 0; x < 4; x++)
		{
			for (int y = 0; y < 8; y++)
			{
				// 动态生成颜色
				QColor color;

				int press = m_MeasureMsg.fsr[x][8-1-y] / 4;
				int maxPress = 400;
				if (press >= maxPress)
				{
					//press = maxPress;
				}

				int displayVal = press * 255 / maxPress;
				if (x == 0 && y == 0)
				{
					//printf("%d %d\r\n", press, displayVal);
				}
				if (displayVal > 255)
				{
					displayVal = 255;
				}
				color = QColor(displayVal, 0, 0);
				changeRectangleColor(x * 8 + y, color);
			}
		}
	}
#endif

	glDrawArrays(GL_TRIANGLES, 0, vertexData.size());
	vbo->release();
	vao->release();
	program->release();
}

void glFsr::setMVP(QMatrix4x4 model, QMatrix4x4 view, QMatrix4x4 projection)
{
	m_model = model;
	m_view = view;
	m_projection = projection;

	program->bind();

	program->setUniformValue("projection", projection);
	program->setUniformValue("view", view);
	program->setUniformValue("model", model);

	program->release();
}

void glFsr::changeRectangleColor(int index, const QColor& color)
{
	QVector3D newColor(
		color.red() / 255.0f,
		color.green() / 255.0f,
		color.blue() / 255.0f
	);
	int colorOffset = 3 * sizeof(GLfloat);
	int colorSize = 3 * sizeof(GLfloat);
	glBufferSubData(GL_ARRAY_BUFFER,
		sizeof(PosColorVec6) * (6 * index + 0) + colorOffset,
		colorSize,
		&newColor);
	glBufferSubData(GL_ARRAY_BUFFER,
		sizeof(PosColorVec6) * (6 * index + 1) + colorOffset,
		colorSize,
		&newColor);
	glBufferSubData(GL_ARRAY_BUFFER,
		sizeof(PosColorVec6) * (6 * index + 2) + colorOffset,
		colorSize,
		&newColor);
	glBufferSubData(GL_ARRAY_BUFFER,
		sizeof(PosColorVec6) * (6 * index + 3) + colorOffset,
		colorSize,
		&newColor);
	glBufferSubData(GL_ARRAY_BUFFER,
		sizeof(PosColorVec6) * (6 * index + 4) + colorOffset,
		colorSize,
		&newColor);
	glBufferSubData(GL_ARRAY_BUFFER,
		sizeof(PosColorVec6) * (6 * index + 5) + colorOffset,
		colorSize,
		&newColor);
}

