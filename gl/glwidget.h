// ... 前面的版权信息保持不变 ...

#ifndef GLWIDGET_H
#define GLWIDGET_H


#include "glLoadStl.h"
#include "glFsr.h"
#include "glAxis.h"
#include "glGrid.h"

#include "glTrackballCamera.h"
#include "inc/modbusMsg.h"



class GLWidget :public QOpenGLWidget, protected QOpenGLFunctions
{
public:
	GLWidget(QWidget* parent = nullptr);

	~GLWidget();
	void setModbusMsg(MeasureMsg_TypeDef msg)
	{
		m_fsr.setModbusMsg(msg);
		m_MeasureMsg = msg;
	}
protected:
	void initializeGL() override;
	void paintGL() override;
	void mousePressEvent(QMouseEvent* event) override;
	void keyPressEvent(QKeyEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void wheelEvent(QWheelEvent* event) override;

	void focusInEvent(QFocusEvent* event) override
	{
		qDebug() << "Widget gained focus";
				QWidget::focusInEvent(event); // 调用父类处理
	}

	void focusOutEvent(QFocusEvent* event) override
	{
		qDebug() << "Widget lost focus";
		QWidget::focusOutEvent(event);
	}
private:
	MeasureMsg_TypeDef m_MeasureMsg;
	void init() { initializeOpenGLFunctions(); }
	void rotateModel(void);

	std::vector<glLoadStl*> m_stlLoaders; // STL加载器列表
	glFsr m_fsr; // FSR对象
	glAxis m_axis; // 坐标轴对象
	glGrid m_grid; // 网格对象

	QMatrix4x4 projection; // 投影矩阵
	QMatrix4x4 view; // 视图矩阵
	QMatrix4x4 model; // 模型矩阵
	float rotationAngle = 0.0f; // 旋转角度

	QVector3D lightPosition;
	QVector3D lightColor;
	
	QVector<QVector3D> stlObjectColors;

	TrackballCamera camera;
	QPoint lastMousePos;

};

#endif