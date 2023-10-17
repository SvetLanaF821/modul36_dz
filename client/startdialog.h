#ifndef STARTDIALOG_H
#define STARTDIALOG_H

#include <QDialog>

namespace Ui {
class StartDialog;
}

class StartDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StartDialog(QWidget *parent = nullptr);
    ~StartDialog();

    void setErrorMessage(const QString &message);   //Установить сообщение в поле
    void setButtonEnabled(bool b);                  //Установить кнопку активной/неактивной

private slots:
    void on_enterButton_clicked();                  //cлот для проверки и отправки данных для входа

signals:
    //Отправить данные для проверки
    void sendData(const QString &login, const QString &password, const QString &host, const QString &port);

private:
    Ui::StartDialog *ui;
    void reject();                                  //функция, срабатывающая при закрытии окна
};

#endif // STARTDIALOG_H
