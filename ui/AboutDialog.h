#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include "ui_AboutDialog.h"

class AboutDialog : public QDialog
{
	Q_OBJECT

public:
	AboutDialog(QWidget *parent = nullptr);
	~AboutDialog();

	void initUi();
signals:
	void btnchicked(int num);
private slots:
	void on_btn_system_close_clicked();

	void on_btn_mbox_btn01_clicked();

	void on_PB_aboutQt_clicked();

private:
	Ui::AboutDialog ui;
};


#endif 
