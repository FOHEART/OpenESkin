#pragma once

#include <QDialog>
#include <QIcon>

#include "ui_openSourceDialog.h"

class openSourceDialog : public QDialog
{
	Q_OBJECT

public:
	openSourceDialog(QWidget *parent = nullptr);
	~openSourceDialog();

private:
	Ui::openSourceDialogClass ui;
};

