#include "openSourceDialog.h"

openSourceDialog::openSourceDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	setWindowIcon(QIcon(":/icons/logo.png"));

	ui.LBL_peach->setOpenExternalLinks(true); // 关键：允许自动打开外部链接
	ui.LBL_qt->setOpenExternalLinks(true); // 关键：允许自动打开外部链接
}

openSourceDialog::~openSourceDialog()
{}

