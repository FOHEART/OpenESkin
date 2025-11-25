#ifndef MAINWIDGET_H
#define MAINWIDGET_H
#include <QMainWindow>

#include <QWidget>
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QMenu>
#include <QSerialPort>

//自定义弹窗
#include "ui/ExitDialog.h"
#include "ui/settingsdialog.h"
#include "ui/frameDisplay.h"
#include "ui/matrixDisplay.h"
#include "ui/openSourceDialog.h"

#include "gl/glwidget.h"

#include "inc/console.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget* parent = 0);
	~MainWindow();

private slots:
	void closeEvent(QCloseEvent* event);

	void changeMboxReturn(int num);

	void on_btn_main_item_1_clicked();

	void on_btn_main_item_2_clicked();

	void on_btn_main_item_3_clicked();

	void on_btn_main_item_4_clicked();

	void on_btn_logout_clicked();

	void on_btn_littleshow_clicked();
	void on_btn_maximum_clicked();

	void on_btn_menu_item_1_clicked();
	void on_btn_menu_item_2_clicked();

	void on_btn_setting_clicked();

	void on_PB_openSerial_clicked();


private:
	Ui::MainWindow* ui;

	//最小化到托盘
	QSystemTrayIcon* trayIcon;
	QMenu* trayIconMenu;
	//窗口管理动作
	QAction* returnNormal;
	QAction* quitAction;

	//顶部导航栏成员变量------------------------------
	QVector<QPushButton*> topBtns; //顶部按钮容器
	void initTopBtns(); //初始化顶部按钮
	void setTopBtnStyleUnselect(); //设置顶部按钮样式-未选中
	void setTopBtnStyleSelect(int index); //设置顶部按钮样式-选中
	QWidget* m_serialLogWidget; //串口日志显示窗口
	//---------------------------------------------



	void initUi();          //Ui界面初始化函数
	void initMember();      //成员变量初始化函数
	void littleShow();      //最小化显示函数

	//串口相关-------------------------------
	bool SerialPort_open();
	void SerialPort_close();
	void handleSerialPortError(QSerialPort::SerialPortError error);
	void SerialPort_init(); //串口初始化函数
	void setUpUIFromSerialStatus(bool isSerialOpen); //根据串口状态设置UI界面
	void SerialPort_fillInfo();
	//---------------------------------------------


private:
	//窗体相关成员变量------------------------------
	/*
	串口设置弹出串口
	*/
	SettingsDialog* m_settingsDialog = nullptr;
	frameDisplay* m_frameDisplay = nullptr;
	matrixDisplay* m_matrixDisplay = nullptr;
	GLWidget* m_glWidget = nullptr;

	/*
	退出确认弹窗
	*/
	ExitDialog* m_exitDialog;
	//自定义窗口通信参数
	int exitDialog_return;
	//---------------------------------------------

	//串口相关成员变量------------------------------
	QSerialPort* m_serial = nullptr;
	Console* m_console = nullptr;
	void writeData(const QByteArray& data);
	void readData();
	std::vector<unsigned char> m_rx_data_list;
	void findAndParseFrame(void);

	//---------------------------------------------
};

#endif // WIDGET_H
