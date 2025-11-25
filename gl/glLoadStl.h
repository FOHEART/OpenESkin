#pragma once
#include "glInclude.h"

class glLoadStl : protected QOpenGLFunctions
{
public:
	glLoadStl();
	~glLoadStl();
	void init() { initializeOpenGLFunctions(); }

	void loadSTL(const QString& filePath);
	void setupGeometry(void);

	void shaderProgramInit(void);
	void setMVP(QMatrix4x4 model, QMatrix4x4 view, QMatrix4x4 projection);
	void draw(void);

	void setlightPosition(const QVector3D& lightPosition)
	{
		m_lightPosition = lightPosition;
		program->bind();
		program->setUniformValue("lightPos", lightPosition);
		program->release();
	}
	void setlightColor(const QVector3D& lightColor)
	{
		m_lightColor = lightColor;
		program->bind();
		program->setUniformValue("lightColor", lightColor);
		program->release();
	}
	

	void setModelMatrix(const QMatrix4x4& model)
	{
		m_model = model;
		program->bind();
		program->setUniformValue("model", model);
		program->release();
	}

	void setViewMatrix(const QMatrix4x4& view)
	{
		m_view = view;
		program->bind();
		program->setUniformValue("view", view);
		program->release();
	}

	void setProjectionMatrix(const QMatrix4x4& projection)
	{
		m_projection = projection;
		program->bind();
		program->setUniformValue("projection", projection);
		program->release();
	}
	int getStlIndex() const { return m_stlIndex; }
	void setStlIndex(int val) { m_stlIndex = val; }
void setObjectColor(const QVector3D& objectColor)
	{
		m_objectColor = objectColor;
		program->bind();
		program->setUniformValue("objectColor", objectColor);
		program->release();
	}

private:
	QVector<QVector3D> m_vertices;
	QVector<QVector3D> m_normals;
	QMatrix4x4 m_projection; // 投影矩阵
	QMatrix4x4 m_view; // 视图矩阵
	QMatrix4x4 m_model; // 模型矩阵
	QVector3D m_lightPosition;
	QVector3D m_lightColor;
	QVector3D m_objectColor;
	

	QOpenGLVertexArrayObject* vao = nullptr; // VAO for cube
	QOpenGLBuffer* vbo = nullptr; // VBO for cube vertices

	QOpenGLShaderProgram* program = nullptr;

	int m_stlIndex = 0;//当前stl文件在所有加载的stl文件中的索引



	//正方体顶点着色
	const char* vertexShaderSource =
		"#version 330 core\n"
		"layout(location = 0) in vec3 aPos;\n"
		"layout(location = 1) in vec3 aNormal;\n"
		"out vec3 FragPos;\n"
		"out vec3 Normal;\n"
		"uniform mat4 model;\n"
		"uniform mat4 view;\n"
		"uniform mat4 projection;\n"
		"void main() {\n"
		"   FragPos = vec3(model * vec4(aPos, 1.0));\n"
		"   Normal = mat3(transpose(inverse(model))) * aNormal;\n"
		"   gl_Position = projection * view * vec4(FragPos, 1.0);\n"
		"}\n"
		;

	//正方体片段着色器
	const char* fragmentShaderSource =
		"#version 330 core\n"
		"in vec3 FragPos;\n"
		"in vec3 Normal;\n"
		"out vec4 FragColor;\n"
		"uniform vec3 objectColor;\n"
		"uniform vec3 lightColor;\n"
		"uniform vec3 lightPos;\n"
		"uniform vec3 viewPos;\n"
		"void main() {\n"
		"   // 环境光照\n"
		"   float ambientStrength = 0.1;\n"
		"   vec3 ambient = ambientStrength * lightColor;\n"
		"   \n"
		"   // 漫反射光照\n"
		"   vec3 norm = normalize(Normal);\n"
		"   vec3 lightDir = normalize(lightPos - FragPos);\n"
		"   float diff = max(dot(norm, lightDir), 0.0);\n"
		"   vec3 diffuse = diff * lightColor;\n"
		"   \n"
		"   // 镜面光照\n"
		"   float specularStrength = 0.5;\n"
		"   vec3 viewDir = normalize(viewPos - FragPos);\n"
		"   vec3 reflectDir = reflect(-lightDir, norm);\n"
		"   float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);\n"
		"   vec3 specular = specularStrength * spec * lightColor;\n"
		"   \n"
		"   // 组合所有光照分量\n"
		"   vec3 result = (ambient + diffuse + specular) * objectColor;\n"
		"   FragColor = vec4(result, 1.0);\n"
		"}\n"
		;
};

