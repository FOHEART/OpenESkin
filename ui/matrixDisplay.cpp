#include "matrixDisplay.h"
#include <QTimer>

matrixDisplay::matrixDisplay(QWidget* parent)
	: QWidget(parent)
{
	//ui.setupUi(this);
	setMinimumWidth(300);

	//reset m_MeasureMsg.fsr
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			m_MeasureMsg.fsr[i][j] = 0;
		}
	}

	// 创建一个定时器
	QTimer* timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, [this]()
	{

		update();  // 调用 update() 触发 paintEvent
	});

	// 启动定时器，每 17 毫秒触发一次
	timer->start(17);
}

matrixDisplay::~matrixDisplay()
{
}

void matrixDisplay::paintEvent(QPaintEvent* event)
{
	//printf("matrixDisplay::paintEvent\r\n");
	Q_UNUSED(event);

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);  // 启用抗锯齿

	// 获取窗口的大小
	int width = this->width();
	int height = this->height();

	// 计算每个方格的大小
	int rows = 8;
	int cols = 4;
	int cellWidth = width / cols;
	int cellHeight = height / rows;

	// 设置画笔颜色
	painter.setPen(Qt::black);

	// 绘制并填充方格
	for (int row = 0; row < rows; ++row)
	{
		for (int col = 0; col < cols; ++col)
		{
			int x = col * cellWidth;
			int y = row * cellHeight;
			QRect rect(x, y, cellWidth, cellHeight);

			// 动态生成颜色
			QColor color;

			int press = m_MeasureMsg.fsr[col][row] / 4;
			int maxPress = 40;
			if (press >= maxPress)
			{
				//press = maxPress;
			}

			int displayVal = press * 255 / maxPress;
			if (row == 0 && col == 0)
			{
				//printf("%d %d\r\n", press, displayVal);
			}
			if (displayVal > 255)
			{
				displayVal = 255;
			}
			color = QColor(displayVal, 0, 0);
			/*if(row==0&&col==0)
			{
				printf("[0,0]=%d color:%d %d %d\r\n",
					press,color.red(),
					 color.green(),
					 color.blue()
					);
			}*/
			// 填充颜色
			painter.fillRect(rect, color);

			// 绘制边框
			painter.drawRect(rect);
		}
	}
}