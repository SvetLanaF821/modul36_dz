#ifndef REGISTRATION_H
#define REGISTRATION_H

#include <QTcpSocket>
#include <QMutex>
#include <QDialog>

namespace Ui {
class Registration;
}

class Registration : public QDialog
{
    Q_OBJECT

public:
    explicit Registration(QWidget *parent = nullptr);
    ~Registration();

    void clearFields();                                 //Очистить поля
    void setErrorMessageReg(const QString &message);    //Установить сообщение в поле
    void setButtonEnabled(bool b);                      //Установить кнопку активной/неактивной

private slots:
    void on_buttonAddUser_clicked();

signals:
    void addUser(const QString &login, const QString &name, const QString &password);   //Добавить пользователя

private:
    Ui::Registration *ui;
};

#endif // REGISTRATION_H
