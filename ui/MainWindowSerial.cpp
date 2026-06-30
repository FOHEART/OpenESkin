#include "MainWindow.h"
#include "ui_MainWidget.h"
#include <QGraphicsDropShadowEffect>
#include <QSerialPortInfo>
#include <QMessageBox>

#include "ui/AboutDialog.h"
#include "inc/commonDef.h"
#include "inc/modbusMsg.h"

#include "inc/crc16_modbus.h"

void MainWindow::SerialPort_init()
{
	SerialPort_fillInfo(); //填充串口信息
	m_console->setEnabled(true);

	ui->HL_frameMsg->addWidget(m_frameDisplay);
	ui->HL_frameMsg->addWidget(m_console);

	m_matrixDisplay->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	ui->HL_matrixDisplay->addWidget( m_matrixDisplay);
	//ui->page_2->layout()->addWidget(m_matrixDisplay);

	m_glWidget = new GLWidget;
	//ui->page->layout()->addWidget(m_glWidget);
	ui->VL_3D->insertWidget(0,m_glWidget);
	
	QObject::connect(m_serial, &QSerialPort::errorOccurred, this, &MainWindow::handleSerialPortError);
	QObject::connect(m_serial, &QSerialPort::readyRead, this, &MainWindow::readData);
	QObject::connect(m_console, &Console::getData, this, &MainWindow::writeData);
	QObject::connect(ui->CBX_serial, &QComboBox::currentTextChanged,
		[=](const QString& text) { 
		m_settingsDialog->setCurrentPortName(ui->CBX_serial->currentText());
	
	});
}

void MainWindow::setUpUIFromSerialStatus(bool isSerialOpen)
{
	if (isSerialOpen)
	{
		ui->CBX_serial->setEnabled(false);
		ui->PB_openSerial->setText(QStringLiteral("关闭"));
	}
	else
	{
		ui->PB_openSerial->setText(QStringLiteral("打开"));
		ui->CBX_serial->setEnabled(true);
	}
}

void MainWindow::SerialPort_fillInfo()
{
	static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");

	ui->CBX_serial->clear();
	QString description;
	QString manufacturer;
	QString serialNumber;
	const auto infos = QSerialPortInfo::availablePorts();
	for (const QSerialPortInfo& info : infos)
	{
		QStringList list;
		description = info.description();
		manufacturer = info.manufacturer();
		serialNumber = info.serialNumber();
		list << info.portName()
			<< (!description.isEmpty() ? description : blankString)
			<< (!manufacturer.isEmpty() ? manufacturer : blankString)
			<< (!serialNumber.isEmpty() ? serialNumber : blankString)
			<< info.systemLocation()
			<< (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : blankString)
			<< (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : blankString);

		ui->CBX_serial->addItem(list.first(), list);
	}
	// 自动选中第一个可用串口，并同步到设置对话框
	if (ui->CBX_serial->count() > 0)
	{
		ui->CBX_serial->setCurrentIndex(0);
		m_settingsDialog->setCurrentPortName(ui->CBX_serial->currentText());
	}
}


bool MainWindow::SerialPort_open()
{
	bool ret = false;
	const SettingsDialog::Settings p = m_settingsDialog->settings();

	fprintf(stdout, "[OpenESkin] 尝试打开串口: '%s' (%s, %s, %s, %s, %s)\n",
		qPrintable(p.name),
		qPrintable(p.stringBaudRate),
		qPrintable(p.stringDataBits),
		qPrintable(p.stringParity),
		qPrintable(p.stringStopBits),
		qPrintable(p.stringFlowControl));
	fflush(stdout);

	if (p.name.isEmpty())
	{
		fprintf(stderr, "[OpenESkin] 错误: 未选择串口端口! 请先连接串口设备或从下拉列表中选择一个端口。\n");
		fflush(stderr);
		QMessageBox::warning(this, tr("未选择串口"),
			tr("请先选择一个串口端口!\n\n"
			   "提示: 如果下拉列表为空，请确认串口设备已连接到电脑。"));
		setUpUIFromSerialStatus(false);
		return false;
	}

	m_serial->setPortName(p.name);
	m_serial->setBaudRate(p.baudRate);
	m_serial->setDataBits(p.dataBits);
	m_serial->setParity(p.parity);
	m_serial->setStopBits(p.stopBits);
	m_serial->setFlowControl(p.flowControl);
	if (m_serial->open(QIODevice::ReadWrite))
	{
		ret = true;
		m_console->putData(QStringLiteral("串口已打开 to %1 : %2, %3, %4, %5, %6\r\n")
			.arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
			.arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));

		fprintf(stdout, "[OpenESkin] 串口打开成功: %s\n", qPrintable(p.name));
		fflush(stdout);

		setUpUIFromSerialStatus(true);
	}
	else
	{
		QString errorStr = m_serial->errorString();
		QSerialPort::SerialPortError errorCode = m_serial->error();

		fprintf(stderr, "[OpenESkin] 串口打开失败!\n");
		fprintf(stderr, "  端口:      %s\n", qPrintable(p.name));
		fprintf(stderr, "  错误码:    %d\n", (int)errorCode);
		fprintf(stderr, "  错误信息:  %s\n", qPrintable(errorStr));
		fprintf(stderr, "  波特率:    %s\n", qPrintable(p.stringBaudRate));
		fprintf(stderr, "  数据位:    %s\n", qPrintable(p.stringDataBits));
		fprintf(stderr, "  校验位:    %s\n", qPrintable(p.stringParity));
		fprintf(stderr, "  停止位:    %s\n", qPrintable(p.stringStopBits));
		fprintf(stderr, "  流控制:    %s\n", qPrintable(p.stringFlowControl));
		fflush(stderr);

		QMessageBox::critical(this, QObject::tr("Error"), p.name + ": " + errorStr);
		ret = false;
		m_console->putData(QStringLiteral("串口无法打开: %1\r\n").arg(m_serial->portName()));

		setUpUIFromSerialStatus(false);
	}

	return ret;
}

void MainWindow::SerialPort_close()
{
	if (!m_serial->isOpen())
		return;

	m_serial->close();
	setUpUIFromSerialStatus(false);

	m_console->putData(QStringLiteral("串口已关闭: %1\r\n").arg(m_serial->portName()));
}

void MainWindow::on_PB_openSerial_clicked()
{
	if (m_serial->isOpen())
	{
		SerialPort_close();
	}
	else
	{
		SerialPort_open();
	}
}

void MainWindow::writeData(const QByteArray& data)
{
	m_serial->write(data);
}

void MainWindow::readData()
{
	const QByteArray data = m_serial->readAll();
	//printf("dataLen:%d\r\n", data.length());

	for (int i = 0; i < data.length(); i++)
	{
		m_rx_data_list.push_back(data[i]);
	}

	findAndParseFrame();
}

int findSequence(const std::vector<unsigned char>& data)
{
	// 遍历向量，注意不要越界访问
	for (size_t i = 0; i < data.size() - 1; ++i)
	{
		if ((data[i] == 0xA5) && (data[i + 1] == 0x5A))
		{
			return static_cast<int>(i); // 返回0xA5的索引
		}
	}
	return -1; // 如果没有找到序列，返回-1
}

void MainWindow::findAndParseFrame(void)
{
	MeasureMsg_TypeDef msg;
	int expectedMsgLen = sizeof(msg);
	while (m_rx_data_list.size() >= expectedMsgLen)
	{
		int index = findSequence(m_rx_data_list);

		if (index == -1)
		{
			return;
		}

		if (index >= 2)
		{

			MeasureMsg_TypeDef msg;
			int expectedMsgLen = sizeof(msg);
			if (m_rx_data_list.size() >= expectedMsgLen)
			{
				QByteArray combineFrame;
				combineFrame.resize(expectedMsgLen);
				for (int i = 0; i < expectedMsgLen; i++)
				{
					combineFrame[i] = m_rx_data_list[index - 2 + i];
				}
				// 计算 CRC16 校验
				memcpy(&msg, combineFrame.data(), combineFrame.length());
				unsigned short crcInFrame = msg.crc16;
				crc16_modbus crc;
				unsigned short crcCalc = crc.CRC16((unsigned char*)combineFrame.data(), sizeof(msg) - 2);
				if (crcInFrame == crcCalc)
				{
					//printf("----------------------fn:%d\r\n", msg.frameNumber);
					m_frameDisplay->setModbusMsg(&msg);
					m_matrixDisplay->setModbusMsg(msg);
					m_glWidget->setModbusMsg(msg);
					QQuaternion q(msg.quat_wxyz[0],
						msg.quat_wxyz[1],
						msg.quat_wxyz[2],
						msg.quat_wxyz[3]
					);
					//glWidgets->rotateBy(q);

					//printf("%d\r\n", msg.fsr[0][0]);
					m_console->putData(combineFrame);
					//remove item from m_rx_data_list from index-2 to index-2+expectedMsgLen
					m_rx_data_list.erase(m_rx_data_list.begin(), m_rx_data_list.begin() + index - 2 + expectedMsgLen);

				}
			}
		}
	}
}

void MainWindow::handleSerialPortError(QSerialPort::SerialPortError error)
{
	switch (error)
	{
		case QSerialPort::NoError:
			break;
		case QSerialPort::DeviceNotFoundError:
			break;
		case QSerialPort::PermissionError:
		{
			//QMessageBox::critical(this, tr("Serial disconnect"), m_serial->errorString());
			SerialPort_close();
		}
		break;
		case QSerialPort::OpenError:
			break;
		case QSerialPort::ParityError:
			break;
		case QSerialPort::FramingError:
			break;
		case QSerialPort::BreakConditionError:
			break;
		case QSerialPort::WriteError:
			break;
		case QSerialPort::ReadError:
			break;
		case QSerialPort::ResourceError:
		{
			//QMessageBox::critical(this, tr("Critical Error"), m_serial->errorString());
			SerialPort_close();
		}
		break;
		case QSerialPort::UnsupportedOperationError:
			break;
		case QSerialPort::UnknownError:
			break;
		case QSerialPort::TimeoutError:
			break;
		case QSerialPort::NotOpenError:
		{
			//QMessageBox::critical(this, tr("Critical Error"), m_serial->errorString());
			SerialPort_close();
		}
		break;
		default:
			break;
	}
}
