#pragma once
#include "glInclude.h"

#include "inc/modbusMsg.h"

typedef struct
{
	float x;
	float y;
	float z;
	float r; // 红色分量
	float g; // 绿色分量
	float b; // 蓝色分量
}PosColorVec6;

 class glFsr: protected QOpenGLFunctions
{
public:
	glFsr() ;
	~glFsr() ;
	void init() { initializeOpenGLFunctions(); }
	void addXYAxisRect(int i, int y);
	void setupGeometry(void);
	void shaderProgramInit(void);
	void draw(void);
	void setModbusMsg(MeasureMsg_TypeDef msg)
	{
		m_MeasureMsg = msg;
	}
	void setMVP(QMatrix4x4 model, QMatrix4x4 view, QMatrix4x4 projection);
	 void changeRectangleColor(int index, const QColor& color);
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


	//阵列
	QVector<PosColorVec6> vertexData;

	QOpenGLShaderProgram* program = nullptr;
	QOpenGLVertexArrayObject* vao = nullptr; // VAO for cube
	QOpenGLBuffer* vbo = nullptr; // VBO for cube vertices
	QMatrix4x4 m_projection; // 投影矩阵
	QMatrix4x4 m_view; // 视图矩阵
	QMatrix4x4 m_model; // 模型矩阵

	//正方体顶点着色器
	const char* vertexShaderSource = R"(
		#version 330 core
		layout(location = 0) in vec3 aPos;
		layout(location = 1) in vec3 aColor;

		out vec3 ourColor;

		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projection;

		void main()
		{
			gl_Position = projection * view * model * vec4(aPos, 1.0);
			ourColor=aColor;
		}
	)";

	//正方体片段着色器
	const char* fragmentShaderSource = R"(
		#version 330 core
		out vec4 FragColor;
		in vec3 ourColor;
		void main()
		{
			FragColor = vec4(ourColor, 1.0);
		}
	)";
private:
	MeasureMsg_TypeDef m_MeasureMsg;
};

