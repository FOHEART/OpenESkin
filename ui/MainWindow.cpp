#include "MainWindow.h"
#include "ui_MainWidget.h"
#include <QGraphicsDropShadowEffect>

#include "ui/AboutDialog.h"
#include "inc/commonDef.h"

MainWindow::MainWindow(QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	m_settingsDialog(new SettingsDialog),
	m_frameDisplay(new frameDisplay(this)),
	m_matrixDisplay(new matrixDisplay(this)),
	m_serial(new QSerialPort),
	m_console(new Console)
{
	ui->setupUi(this);

	this->setProperty("canMove", true);
	this->initUi();
	this->initMember();

	on_btn_main_item_1_clicked(); //默认选中第一个按钮

	SerialPort_init();
}

MainWindow::~MainWindow()
{
	delete m_console;
	delete m_serial;
	delete m_settingsDialog;
	delete ui;
}

void MainWindow::initTopBtns()
{
	topBtns.push_back(ui->btn_main_item_1);
	topBtns.push_back(ui->btn_main_item_2);
	topBtns.push_back(ui->btn_main_item_3);
	topBtns.push_back(ui->btn_main_item_4);

	//iterator 
	setTopBtnStyleUnselect();
}

void MainWindow::setTopBtnStyleUnselect()
{
	//iterator 
	for (auto it = topBtns.begin(); it != topBtns.end(); ++it)
	{
		(*it)->setStyleSheet(
			"QPushButton {"
			"    background-color: #e0e0e0;"
			"    color: #808080;"
			"    border-radius: 15px;"
			"    padding: 5px 10px;"
			"}"
			"QPushButton:hover {"
			"    background-color: #d0d0d0;"
			"}"
			"QPushButton:pressed {"
			"    background-color: #c0c0c0;"
			"}"
		);
	}
}

void MainWindow::setTopBtnStyleSelect(int index)
{
	topBtns[index]->setStyleSheet(
		"QPushButton {"
		"    background-color: #2f88ff;"
		"    color: #000000;"
		"    border-radius: 15px;"
		"    padding: 5px 10px;"
		"}"
	);
}

/**********************************************************初始化函数****************************************************************/

void MainWindow::initUi()
{
	//初始化窗口边框
	this->setAttribute(Qt::WA_TranslucentBackground, true);
	this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
	QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(this);
	shadow->setOffset(0, 0);
	shadow->setColor(QColor("#444444"));
	shadow->setBlurRadius(16);
	//ui->w_bg->setGraphicsEffect(shadow);
	//ui->lay_bg->setMargin(12);

	//Logo
	QPixmap logo(":/icons/logo.png");
	ui->lab_logo->setPixmap(logo);

	//itemLogo
	QPixmap item1(":/icons/usb.png");
	ui->lab_item_01->setPixmap(item1);

	//this->showMaximized();

	ui->LBL_mainHeader->setText(APP_HEADER);

	ui->btn_logout->setStyleSheet(
		"QPushButton {"
		"background: transparent;"
		"border: none;"
		"}"
		"QPushButton:hover {"
		"background-color: #ff0000;"
		"border: none;"
		"}"
	);

	ui->btn_littleshow->setStyleSheet(
		"QPushButton {"
		"background: transparent;"
		"border: none;"
		"}"
		"QPushButton:hover {"
		"background-color: #cccccc;"
		"border: none;"
		"}"
	);

	ui->btn_maximum->setStyleSheet(
		"QPushButton {"
		"background: transparent;"
		"border: none;"
		"    image: url(:/icons/maxNormal.png);"  // 默认图标
		"}"
		"QPushButton:hover {"
		"background-color: #cccccc;"
		"border: none;"
		"    image: url(:/icons/maxNormal.png);"  // 默认图标
		"}"
	);
}

void MainWindow::initMember()
{
	this->m_exitDialog = new ExitDialog();
	connect(m_exitDialog, SIGNAL(btnchicked(int)), this, SLOT(changeMboxReturn(int)));

	//最小化到托盘
	QIcon icon = QIcon(":/icons/logo.png");//设置最小图标
	trayIcon = new QSystemTrayIcon(this);
	//trayIcon->setIcon(icon);
	trayIcon->setToolTip("Peach"); //提示文字
	//添加托盘列表项(还原与退出)Noto Sans SC
	returnNormal = new QAction(" Show", this);
	//returnNormal->setFont(QFont("Arial", 9));
	returnNormal->setObjectName("returnNormal");
	//returnNormal->setIcon(QIcon(":/icons/show.png"));
	quitAction = new QAction(" Quit", this);
	//quitAction->setFont(QFont("Arial", 9));
	quitAction->setObjectName("quitAction");
	//quitAction->setIcon(QIcon(":/icons/out.png"));
	connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));           //绑定槽函数退出
	connect(returnNormal, SIGNAL(triggered()), this, SLOT(showNormal()));   //绑定槽函数还原界面

	//创建托盘菜单(必须先创建动作，后添加菜单项，还可以加入菜单项图标美化)
	trayIconMenu = new QMenu(this);
	trayIconMenu->addAction(returnNormal);
	trayIconMenu->addAction(quitAction);
	trayIcon->setContextMenu(trayIconMenu);

	initTopBtns();
}

/**********************************************************自定义函数****************************************************************/

void MainWindow::littleShow()
{
	this->hide();//隐藏主窗口
	trayIcon->show();//显示托盘

	//显示到系统提示框的信息
	QString title = "Peach";
	QString text = QObject::tr(u8"正自动在后台运行");
	trayIcon->showMessage(title, text, QSystemTrayIcon::Information, 3000); //此参数为提示时长
}

/**********************************************************控件槽函数****************************************************************/

void MainWindow::closeEvent(QCloseEvent* event)
{
	m_exitDialog->setIcon("question");
	m_exitDialog->setBodyText(QObject::tr(u8"退出程序？"));
	QString minStr = QObject::tr(u8"最小化");

	QString exitStr = QObject::tr(u8"退出");
	m_exitDialog->setButtonText(minStr, exitStr);
	m_exitDialog->exec();
	if (exitDialog_return == 0)
	{
		event->ignore();
	}
	else if (exitDialog_return == 1)
	{
		event->ignore();
		littleShow();
	}
}

void MainWindow::changeMboxReturn(int num)
{
	this->exitDialog_return = num;
}

void MainWindow::on_btn_main_item_1_clicked()
{
	ui->sw_main->setCurrentIndex(0);
	setTopBtnStyleUnselect();

	setTopBtnStyleSelect(0);

}

void MainWindow::on_btn_main_item_2_clicked()
{
	ui->sw_main->setCurrentIndex(1);

	setTopBtnStyleUnselect();

	setTopBtnStyleSelect(1);
}

void MainWindow::on_btn_main_item_3_clicked()
{
	ui->sw_main->setCurrentIndex(2);
	setTopBtnStyleUnselect();

	setTopBtnStyleSelect(2);
}

void MainWindow::on_btn_main_item_4_clicked()
{
	ui->sw_main->setCurrentIndex(3);
	setTopBtnStyleUnselect();

	setTopBtnStyleSelect(3);
}

void MainWindow::on_btn_logout_clicked()
{
	close();
}

void MainWindow::on_btn_littleshow_clicked()
{
	showMinimized();
}

void MainWindow::on_btn_maximum_clicked()
{
	if (this->isMaximized())
	{
		ui->btn_maximum->setStyleSheet(
			"QPushButton {"
			"background: transparent;"
			"border: none;"
			"    image: url(:/icons/maxNormal.png);"  // 默认图标
			"}"
			"QPushButton:hover {"
			"background-color: #cccccc;"
			"border: none;"
			"}"
		);
		this->showNormal();
	}
	else
	{
		ui->btn_maximum->setStyleSheet(
			"QPushButton {"
			"background: transparent;"
			"border: none;"
			"    image: url(:/icons/maxNormal2.png);"  // 默认图标
			"}"
			"QPushButton:hover {"
			"background-color: #cccccc;"
			"border: none;"
			"}"
		);
		this->showMaximized();
	}
}

void MainWindow::on_btn_menu_item_1_clicked()
{
	AboutDialog* about = new AboutDialog(this);

	about->exec();
}

void MainWindow::on_btn_menu_item_2_clicked()
{
	openSourceDialog* dlg = new openSourceDialog(this);

	dlg->exec();
}

void MainWindow::on_btn_setting_clicked()
{
	m_settingsDialog->setCurrentPortName(ui->CBX_serial->currentText());
	m_settingsDialog->show();
}
