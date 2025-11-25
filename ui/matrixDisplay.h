#pragma once

#include <QWidget>
#include "ui_matrixDisplay.h"
#include <QPainter>
#include <QPaintEvent>
#include "inc/modbusMsg.h"


class matrixDisplay : public QWidget
{
	Q_OBJECT

public:
	matrixDisplay(QWidget *parent = nullptr);
	~matrixDisplay();
	void setModbusMsg(MeasureMsg_TypeDef& msg)
	{
		m_MeasureMsg = msg;
	}
protected:
	void paintEvent(QPaintEvent* event) override;
protected:
	void resizeEvent(QResizeEvent* event) override
	{
		int height = event->size().height();
		setFixedSize(static_cast<int>(height * aspect_ratio), height);
	}

private:
	Ui::matrixDisplayClass ui;
	MeasureMsg_TypeDef m_MeasureMsg;
	double aspect_ratio=4.0/8.0; // 宽高比（宽度/高度）
};
