#include "frameDisplay.h"

frameDisplay::frameDisplay(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);


	// 定义行列数
	int rows = 8;
	int columns = 4;

	// 循环创建 QLabel 并添加到 QGridLayout 中
	for (int row = 0; row < rows; ++row)
	{
		for (int col = 0; col < columns; ++col)
		{
			// 创建一个新的 QLabel
			QString labelText = QString("0");
			QLabel* label = new QLabel(labelText, this);
			label->setMinimumSize(30, 25);
			label->setMaximumSize(30, 25);
			// 设置一些样式以使标签更易于查看
			label->setAlignment(Qt::AlignCenter);
			label->setStyleSheet("border: 1px solid black;");

			// 将 QLabel 添加到 QGridLayout 中
			ui.GL_fsr->addWidget(label, row, col);
		}
	}
	m_thermometre = new thermometerWidget(this);
	m_thermometre->setMinimumWidth(100);
	m_thermometre->setMaximumWidth(100);
	ui.GL_tmpr->addWidget(m_thermometre, 0, 0);

}

frameDisplay::~frameDisplay()
{
}

void frameDisplay::updateFsr(unsigned short* fsr, int column, int row)
{


	for (int i = 0; i < column; i++)
	{
		for (int j = 0; j < row; j++)
		{
			QLayoutItem* item = ui.GL_fsr->itemAtPosition(j, i);
			if (item && item->widget())
			{
				QLabel* label = qobject_cast<QLabel*>(item->widget());
				if (label)
				{
					label->setText(QString::number(fsr[i * row + j]));
				}
			}
		}
	}


}

void frameDisplay::updateTmpr(float tmpr)
{
	m_thermometre->setValue(tmpr);
}
