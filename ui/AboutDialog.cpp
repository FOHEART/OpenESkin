#include "AboutDialog.h"
#include <QPixmap>
//阴影类包含头文件
#include <QGraphicsDropShadowEffect>
#include "inc/commonDef.h"

AboutDialog::AboutDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	this->setProperty("canMove", true);
	this->initUi();
}

AboutDialog::~AboutDialog()
{}

void AboutDialog::initUi()
{
	//设置窗体透明
	this->setAttribute(Qt::WA_TranslucentBackground, true);
	//设置无边框
	this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	//实例阴影shadow
	QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(this);
	//设置阴影距离
	shadow->setOffset(0, 0);
	//设置阴影颜色
	shadow->setColor(QColor("#444444"));
	//设置阴影圆角
	shadow->setBlurRadius(16);
	//给嵌套QDialog设置阴影
	ui.w_bg->setGraphicsEffect(shadow);
	//给垂直布局器设置边距(此步很重要, 设置宽度为阴影的宽度)
	//ui.lay_bg->setMargin(12);

	//
	ui.LBL_header->setText(QObject::tr(u8"关于"));
	ui.LBL_header_2->setText(APP_HEADER);
	ui.LBL_version->setText(QObject::tr(u8"版本 0.0.9"));
	ui.LBL_version->setStyleSheet("color: gray;");  // 或 "color: #808080;"/"color: rgb(128, 128, 128);"
	ui.btn_mbox_btn01->setText(QObject::tr(u8"关闭"));

}

void AboutDialog::on_btn_system_close_clicked()
{
	//发送信号
	btnchicked(0);
	//关闭窗口
	close();
}

void AboutDialog::on_btn_mbox_btn01_clicked()
{
	//发送信号
	btnchicked(1);
	//关闭窗口
	close();
}

void AboutDialog::on_PB_aboutQt_clicked()
{
	qApp->aboutQt();
}

