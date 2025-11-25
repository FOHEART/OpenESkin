#include "glLoadStl.h"

glLoadStl::glLoadStl()
{
	
}

glLoadStl::~glLoadStl()
{

}

void glLoadStl::loadSTL(const QString& filePath)
{
	QElapsedTimer timer;
	timer.start();

	m_vertices.clear();
	m_normals.clear();

	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qWarning() << "Could not open file:" << filePath;
		return;
	}

	QTextStream in(&file);
	QString line;
	QVector<QVector3D> currentVertices;
	QVector3D currentNormal;

	while (!in.atEnd())
	{
		line = in.readLine().trimmed();

		if (line.startsWith("facet normal"))
		{
			QStringList parts = line.split(" ", QString::SkipEmptyParts);
			if (parts.size() >= 5)
			{
				currentNormal = QVector3D(
					parts[2].toFloat(),
					parts[3].toFloat(),
					parts[4].toFloat()
				);
			}
		}
		else if (line.startsWith("vertex"))
		{
			QStringList parts = line.split(" ", QString::SkipEmptyParts);
			if (parts.size() >= 4)
			{
				currentVertices.append(QVector3D(
					parts[1].toFloat() / 10,
					parts[2].toFloat() / 10,
					parts[3].toFloat() / 10
				));
			}
		}
		else if (line.startsWith("endfacet"))
		{
			// For each triangle, add vertices and normal for each vertex
			if (currentVertices.size() == 3)
			{
				for (int i = 0; i < 3; i++)
				{
					m_vertices.append(currentVertices[i]);
					m_normals.append(currentNormal);
				}
			}
			currentVertices.clear();
		}
	}

	file.close();

	qDebug() << "Loaded" << m_vertices.size() / 3 << "triangles in" << timer.elapsed() << "ms";
}

void glLoadStl::setupGeometry(void)
{
	//创建定点数据 位置+颜色
	QVector<QVector3D> vertexData;
	for (int i = 0; i < m_vertices.size(); i++)
	{
		vertexData.append(m_vertices[i]);
		vertexData.append(m_normals[i]);
	}
	//设置立方体的VA0 VB0
	vao = new QOpenGLVertexArrayObject();
	vao->create();
	vao->bind();

	vbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
	vbo->create();
	vbo->bind();
	vbo->allocate(vertexData.constData(), vertexData.size() * sizeof(QVector3D));

	program->bind();
	program->enableAttributeArray(0); // 位置属性位置0
	program->setAttributeBuffer(0, GL_FLOAT, 0, 3, 2 * sizeof(QVector3D)); // 每个顶点3个float，位置属性偏移0
	program->enableAttributeArray(1); // 颜色属性位置1
	program->setAttributeBuffer(1, GL_FLOAT, sizeof(QVector3D), 3, 2 * sizeof(QVector3D)); // 每个顶点3个float，颜色属性偏移3个float
	program->release();

	vbo->release();
	vao->release();
}

void glLoadStl::shaderProgramInit(void)
{
	program = new QOpenGLShaderProgram();
	program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
	program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
	program->link();
}

void glLoadStl::setMVP(QMatrix4x4 model, QMatrix4x4 view, QMatrix4x4 projection)
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

void glLoadStl::draw(void)
{
	//setModelMatrix(m_model);
	//setViewMatrix(m_view);
	program->bind();
	vao->bind();
	glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());
	vao->release();
	program->release();
}
