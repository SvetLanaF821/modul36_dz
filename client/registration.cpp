#include "registration.h"
#include "ui_registration.h"

#include <QString>

Registration::Registration(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Registration)
{
    ui->setupUi(this);
}

Registration::~Registration()
{
    delete ui;
}

//Очистить поля
void Registration::clearFields()
{
    ui->lineAddLogin->clear();
    ui->lineAddName->clear();
    ui->lineAddPass->clear();
}

//Установить сообщение в поле
void Registration::setErrorMessageReg(const QString &message)
{
    //Добавление надписи об ошибке
    ui->resultLabel->setStyleSheet("color: red;");
    ui->resultLabel->setText(message);
}

//Установить кнопку активной/неактивной
void Registration::setButtonEnabled(bool b)
{
    ui->buttonAddUser->setEnabled(b);
}

void Registration::on_buttonAddUser_clicked()
{
    //Очищаем поле с сообщением
    ui->resultLabel->setText("");

    //Устанавливаем стили полей по умолчанию
    ui->lineAddLogin->setStyleSheet("border:0px;");
    ui->lineAddLogin->setStyleSheet("border-bottom:1px solid black;");

    ui->lineAddName->setStyleSheet("border:0px;");
    ui->lineAddName->setStyleSheet("border-bottom:1px solid black;");

    ui->lineAddPass->setStyleSheet("border:0px;");
    ui->lineAddPass->setStyleSheet("border-bottom:1px solid black;");

    //Считываем значения
    QString login = ui->lineAddLogin->text();
    QString name = ui->lineAddName->text();
    QString pass = ui->lineAddPass->text();

    //Проверяем есть ли пустые поля
    if(login.isEmpty() || name.isEmpty() || pass.isEmpty()){
        //Если да, то делаем поле с красной границей
        if(login.isEmpty()){
            ui->lineAddLogin->setStyleSheet("border:1px solid red;");
        }
        if(name.isEmpty()){
            ui->lineAddName->setStyleSheet("border:1px solid red;");
        }
        if(pass.isEmpty()){
            ui->lineAddPass->setStyleSheet("border:1px solid red;");
        }
        ui->resultLabel->setStyleSheet("color: red;");
        ui->resultLabel->setText(tr("Некоторые поля пустые"));
        return;
    }

    //Отправляем сигнал
    emit addUser(login, name, pass);
}
