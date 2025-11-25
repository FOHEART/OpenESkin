/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "inc/console.h"

#include <QScrollBar>
#include <QElapsedTimer>
#include <QDateTime>

Console::Console(QWidget *parent) :
    QPlainTextEdit(parent)
{
    document()->setMaximumBlockCount(100);
    QPalette p = palette();
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::green);
    setPalette(p);

	setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
}

QString Console::byteArrayToHexAndAscii(const QByteArray& data)
{
	QString result;
	int lineLength = 8;  // 每行显示的字节数
	int dataSize = data.size();

	for (int i = 0; i < dataSize; i += lineLength)
	{
		// 1. 输出偏移地址
		result += QString("0x%1  ").arg(i, 4, 16, QChar('0')).toUpper();

		// 2. 输出 16 进制数据
		for (int j = 0; j < lineLength; ++j)
		{
			if (i + j < dataSize)
			{
				result += QString("%1 ").arg((unsigned char)data[i + j], 2, 16, QChar('0')).toUpper();
			}
			else
			{
				result += "   ";  // 填充空格
			}
		}

		result += "  ";  // 分隔符

		// 3. 输出 ASCII 数据
		for (int j = 0; j < lineLength; ++j)
		{
			if (i + j < dataSize)
			{
				char c = data[i + j];
				if (c >= 32 && c <= 126)
				{  // 可打印字符范围
					result += c;
				}
				else
				{
					result += '.';  // 非可打印字符用 '.' 替代
				}
			}
			else
			{
				result += ' ';  // 填充空格
			}
		}

		result += "\n";  // 换行
	}

	return result;
}

void Console::putData(const QByteArray &data)
{
	if (contentLength > contentMaxLength)
	{
		clear(); contentLength = 0;
	}
	QString displayContent;
	// 获取当前时间
	QDateTime currentDateTime = QDateTime::currentDateTime();
	// 精确到毫秒的时间字符串
	QString formattedTime = currentDateTime.toString("[RX->]hh:mm:ss.zzz\r\n");
	displayContent += formattedTime;
	
	QString str = byteArrayToHexAndAscii(data);
	displayContent += str;
    insertPlainText(displayContent);
	contentLength += displayContent.length();

    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());
}
void Console::putData(const QString& data)
{
	if (contentLength > contentMaxLength)
	{
		clear(); contentLength = 0;
	}
	QString displayContent;
	// 获取当前时间
	QDateTime currentDateTime = QDateTime::currentDateTime();
	// 精确到毫秒的时间字符串
	QString formattedTime = currentDateTime.toString("[LOG]hh:mm:ss.zzz\r\n");
	displayContent += formattedTime;

	QString str = data;
	displayContent += str;
	insertPlainText(displayContent);
	contentLength += displayContent.length();

	QScrollBar* bar = verticalScrollBar();
	bar->setValue(bar->maximum());
}

void Console::setLocalEchoEnabled(bool set)
{
    m_localEchoEnabled = set;
}

void Console::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Backspace:
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Up:
    case Qt::Key_Down:
        break;
    default:
        if (m_localEchoEnabled)
            QPlainTextEdit::keyPressEvent(e);
        emit getData(e->text().toLocal8Bit());
    }
}

void Console::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    setFocus();
}

void Console::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
}

void Console::contextMenuEvent(QContextMenuEvent *e)
{
	QMenu* menu = createStandardContextMenu();
	menu->exec(e->globalPos());
	delete menu;
}
