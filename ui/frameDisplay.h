#pragma once

#include <QWidget>
#include "ui_frameDisplay.h"
#include "inc/modbusMsg.h"
#include "thermometerWidget.h"

class frameDisplay : public QWidget
{
	Q_OBJECT

public:
	frameDisplay(QWidget* parent = nullptr);
	~frameDisplay();

	void setModbusMsg(MeasureMsg_TypeDef* msg)
	{
		if (isVisible())
		{
			// 当前widget在界面上可见
			//printf("frameDisplay::visible\n");
		}
		else
		{
			//printf("frameDisplay::no visible\n");
			return;
			// 当前widget不可见（可能被隐藏或不在激活的标签页中）
		}
		ui.LE_addr->setText(QString::number(msg->addr));
		ui.LE_funcCode->setText(QString::number(msg->funcCode));
		ui.LE_frameNumber->setText(QString::number(msg->frameNumber));
		ui.LE_quat_w->setText(QString::number(msg->quat_wxyz[0], 'g', 4));
		ui.LE_quat_x->setText(QString::number(msg->quat_wxyz[1], 'g', 4));
		ui.LE_quat_y->setText(QString::number(msg->quat_wxyz[2], 'g', 4));
		ui.LE_quat_z->setText(QString::number(msg->quat_wxyz[3], 'g', 4));
		ui.LE_temperature->setText(QString::number(msg->temperature, 'g', 4));

		updateFsr(&msg->fsr[0][0], 4, 8);
		updateTmpr(msg->temperature);
	}

private:
	Ui::frameDisplayClass ui;
	thermometerWidget* m_thermometre;
	void updateFsr(unsigned short* fsr, int column, int row);
	void updateTmpr(float tmpr);
};
