#ifndef EXITDIALOG_H
#define EXITDIALOG_H

#include <QDialog>
#include <QString>

/*
mbox_return返回值：
        0(关闭了弹窗)、1(点击了第一个按钮)、2(点击了第二个按钮)
调用示例：
        myMbox->setIcon("question");                      //设置图标，可选值：check（确定）、question（疑问）、warn（警告）、error（报错）
        myMbox->setBodyText("请选择退出方式");               //设置正文内容
        myMbox->setButtonText("确定");                     //设置单个按钮
        myMbox->setButtonText("确定","取消");               //设置两个按钮
        myMbox->exec();                                   //激活弹窗
*/

namespace Ui {
class ExitDialog;
}

class ExitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExitDialog(QDialog *parent = 0);
    ~ExitDialog();

    void setIcon(QString type);
    void setBodyText(QString text);
    void setButtonText(const QString& text);
    void setButtonText(const QString &text1, const QString &text2);

signals:
    void btnchicked(int num);

private slots:
    void on_btn_system_close_clicked();

    void on_btn_mbox_btn01_clicked();

    void on_btn_mbox_btn02_clicked();

private:
    Ui::ExitDialog *ui;

    void initUi();
};

#endif 
