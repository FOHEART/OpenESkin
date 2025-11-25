#include "glwidget.h"
#include <QCoreApplication>


GLWidget::GLWidget(QWidget* parent /*= nullptr*/) :QOpenGLWidget(parent),
lightPosition(QVector3D(0.0f, 10.0f, 0.0f)),
lightColor(QVector3D(1.0f, 1.0f, 1.0f))
{
	setFocusPolicy(Qt::StrongFocus);
	stlObjectColors.push_back(QVector3D(
		100.0f / 255.0f,
		100.0f / 255.0f,
		100.0f / 255.0f
	));
	stlObjectColors.push_back(QVector3D(
		210.0f / 255.0f,
		210.0f / 255.0f,
		210.0f / 255.0f
	));

	stlObjectColors.push_back(QVector3D(
		170.0f / 255.0f,
		132.0f / 255.0f,
		62.0f / 255.0f
	));

	stlObjectColors.push_back(QVector3D(
		200.0f / 255.0f,
		200.0f / 255.0f,
		200.0f / 255.0f
	));
	for (int i = 0; i < 30; i++)
	{
		stlObjectColors.push_back(QVector3D(
			200.0f / 255.0f,
			200.0f / 255.0f,
			200.0f / 255.0f
		));
	}
	QString appDir = QCoreApplication::applicationDirPath();
	std::vector<std::string> loaders;

	loaders.push_back(appDir.toStdString() + "/models/mc2010/part1.stl");
	loaders.push_back(appDir.toStdString() + "/models/mc2010/part2.stl");
	loaders.push_back(appDir.toStdString() + "/models/mc2010/part3.stl");
	loaders.push_back(appDir.toStdString() + "/models/mc2010/part4.stl");
	/*loaders.push_back(appDir.toStdString() + "/models/handskeleton/1.stl");
	loaders.push_back(appDir.toStdString() + "/models/handskeleton/2.stl");
	loaders.push_back(appDir.toStdString() + "/models/handskeleton/3.stl");
	loaders.push_back(appDir.toStdString() + "/models/handskeleton/4.stl");
	loaders.push_back(appDir.toStdString() + "/models/handskeleton/5.stl");
	loaders.push_back(appDir.toStdString() + "/models/handskeleton/6.stl");
	loaders.push_back(appDir.toStdString() + "/models/handskeleton/7.stl");
	loaders.push_back(appDir.toStdString() + "/models/handskeleton/8.stl");
	loaders.push_back(appDir.toStdString() + "/models/handskeleton/9.stl");
	loaders.push_back(appDir.toStdString() + "/models/handskeleton/10.stl");
	loaders.push_back(appDir.toStdString() + "/models/handskeleton/11.stl");
	loaders.push_back(appDir.toStdString() + "/models/handskeleton/12.stl");
	loaders.push_back(appDir.toStdString() + "/models/handskeleton/13.stl");
	loaders.push_back(appDir.toStdString() + "/models/handskeleton/14.stl");
	loaders.push_back(appDir.toStdString() + "/models/handskeleton/15.stl");
	loaders.push_back(appDir.toStdString() + "/models/handskeleton/16.stl");
	loaders.push_back(appDir.toStdString() + "/models/handskeleton/17.stl");
	loaders.push_back(appDir.toStdString() + "/models/handskeleton/18.stl");
	loaders.push_back(appDir.toStdString() + "/models/handskeleton/19.stl");
	loaders.push_back(appDir.toStdString() + "/models/handskeleton/20.stl");
	loaders.push_back(appDir.toStdString() + "/models/handskeleton/21.stl");
	loaders.push_back(appDir.toStdString() + "/models/handskeleton/22.stl");
	loaders.push_back(appDir.toStdString() + "/models/handskeleton/23.stl");
	loaders.push_back(appDir.toStdString() + "/models/handskeleton/24.stl");
	loaders.push_back(appDir.toStdString() + "/models/handskeleton/25.stl");
	loaders.push_back(appDir.toStdString() + "/models/handskeleton/26.stl");
	loaders.push_back(appDir.toStdString() + "/models/handskeleton/27.stl");*/

	for (const auto& loader : loaders)
	{
		glLoadStl* stlLoader = new glLoadStl();
		stlLoader->loadSTL(QString::fromStdString(loader));
		m_stlLoaders.push_back(stlLoader);
	}

	camera.rotate(0, 20);
}


GLWidget::~GLWidget()
{
	//release m_stlLoaders
	for (size_t i = 0; i < m_stlLoaders.size(); ++i)
	{
		delete m_stlLoaders[i];
	}
}

void GLWidget::initializeGL()
{
	init();
	glClearColor(
		240.0f / 255.0f,
		240.0f / 255.0f,
		240.0f / 255.0f,
		1.0f);
	//设置投影矩阵
	projection.setToIdentity();
	projection.perspective(45.0f, width() / float(height()), 0.1f, 100.0f);
	//模型视图矩阵
	view.setToIdentity();
	view.translate(0.0f, 0.0f, -5.0f); // 向后移动5个单位
	model.setToIdentity();

	//--------------

	//iterate m_stlLoaders
	for (size_t i = 0; i < m_stlLoaders.size(); ++i)
	{
		m_stlLoaders[i]->init();
		m_stlLoaders[i]->shaderProgramInit();
		//设置正方形顶点数据 位置和颜色
		m_stlLoaders[i]->setupGeometry();
		m_stlLoaders[i]->setMVP(model, view, projection);
		m_stlLoaders[i]->setlightPosition(lightPosition);
		m_stlLoaders[i]->setlightColor(lightColor);
		m_stlLoaders[i]->setObjectColor(stlObjectColors[i]);
		m_stlLoaders[i]->setStlIndex(i);
	}

	//压力阵列着色器程序
	m_fsr.init();
	m_fsr.shaderProgramInit();
	m_fsr.setupGeometry();
	m_fsr.setMVP(model, view, projection);

	//地面网格 
	m_grid.init();
	m_grid.shaderProgramInit();
	m_grid.initGrid();
	//坐标轴
	m_axis.init();
	m_axis.shaderProgramInit();
	m_axis.initAxis();
}

void GLWidget::rotateModel(void)
{
	QQuaternion tempQ(
		m_MeasureMsg.quat_wxyz[0],
		m_MeasureMsg.quat_wxyz[1],
		m_MeasureMsg.quat_wxyz[2],
		m_MeasureMsg.quat_wxyz[3]
	);

	QQuaternion q2 = QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, -90.0f);	

	QQuaternion	tempQ2 = QQuaternion(tempQ.scalar(), tempQ.x(), tempQ.y(), tempQ.z()); // 反转X、Y、Z分量
	tempQ2 = q2 * tempQ2;
	
	model.setToIdentity();
	model.rotate(tempQ2);
}


void GLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	view = camera.getViewMatrix();
#if 0
	//旋转
	rotationAngle += 0.5f; // 每次绘制增加0.5度
	if (rotationAngle >= 360.0f) rotationAngle -= 360.0f; // 保持在0-360度之间

	model.setToIdentity();
	model.rotate(rotationAngle, QVector3D(1.0f, 1.0f, 1.0f));
#else if

	rotateModel();
#endif


	//iterate m_stlLoaders
	for (size_t i = 0; i < m_stlLoaders.size(); ++i)
	{
		m_stlLoaders[i]->setModelMatrix(model);
		m_stlLoaders[i]->setViewMatrix(view);
		m_stlLoaders[i]->setProjectionMatrix(projection);
		m_stlLoaders[i]->draw();
	}

	//阵列
	//旋转


	//绘制压力显示部分
	m_fsr.setModelMatrix(model);
	m_fsr.setViewMatrix(view);
	m_fsr.draw();

	//绘制地板
	m_grid.setMVP(model, view, projection);
	m_grid.draw();

	//绘制坐标轴
	m_axis.setMVP(model, view, projection);
	m_axis.draw();
	update();

	//static int glDrawCounter = 0;
	//printf("gl draw:%d\r\n", glDrawCounter++);
}

void GLWidget::mousePressEvent(QMouseEvent* event)
{
	lastMousePos = event->pos();
	
}

void GLWidget::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_Space)
	{
		camera.reset();
		update();
	}
}

void GLWidget::mouseMoveEvent(QMouseEvent* event)
{
	QPoint currentPos = event->pos();
	float dx = (currentPos.x() - lastMousePos.x());
	float dy = (currentPos.y() - lastMousePos.y());

	if (event->buttons() & Qt::LeftButton)
	{
		camera.rotate(dx, dy);
		//printf("Mouse Pressed at: (%f, %f)\n", dx, dy);
		update();
	}
	else if (
		(event->buttons() & Qt::RightButton) ||
		(event->buttons() & Qt::MiddleButton)
		)
	{
		camera.pan(dx * 0.01f, -dy * 0.01f);
		update();
	}

	lastMousePos = currentPos;
}

void GLWidget::wheelEvent(QWheelEvent* event)
{
	// 获取滚轮滚动量
	QPoint numDegrees = event->angleDelta() / 2;

	if (!numDegrees.isNull())
	{
		// 计算缩放因子 (根据当前距离调整灵敏度)
		float sensitivity = qMax(0.1f, camera.getDistance() * 0.05f);
		float zoomDelta = -numDegrees.y() * sensitivity * 0.01f;

		// 应用缩放
		camera.zoom(zoomDelta);
		update();
	}

	event->accept();
}
