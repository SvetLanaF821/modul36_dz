#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QVector>
#include <QTcpServer>
#include <QTcpSocket>
#include <memory>
#include "user.h"
#include "message.h"
#include <QtSql>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    MainWindow(const int port, const QString &dbName, QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QVector<User> users_;                                             //вектор пользователей
    QVector<Message> messages_;                                       //вектор сообщений
    std::unique_ptr<QTcpServer> tcpServer_;                           //указатель для работы с TCP-сервером
    quint16 nextBlockSize_;                                           //переменная для обмена сообщениями по сети
    int port_;                                                        //порт для соединения
    QString dbName_;                                                  //имя база данны
    QSqlDatabase db_;                                                 //Объект, который нужен для работы с БД
    std::shared_ptr<QTcpSocket*> pSocket_;                            //сокет
    QString currentUser_;                                             //текущий пользователь

    void addNewUser(const QString &login, const QString &name, const QString &pass);      //добавить нового пользователя
    void addToUsersTable(const QString &login, const bool isBlocked, const bool isBanned);//добавить строку в таблицу пользователей
    void addToMessagesTable(const QString &from, const QString &to, const QString &text, const QString &datetime);//добавить строку в таблицу сообщений
    void connectToDb();                                               //подключиться к БД
    void addToLog(const QString &str);                                //вывести сообщение в поле журнала
    void addToLogRed(const QString &str);                             //вывести сообщение в поле журнала красным цветом
    void addToLogGreen(const QString &str);                           //вывести сообщение в поле журнала зеленым цветом
    void createDataBase();                                            //создать БД
    void readUsersFromBd();                                           //прочитать всех пользователей из БД
    void readMessagesFromBd();                                        //прочитать все сообщения из БД
    void addUserToBd(const QString &login, const QString &pass, const QString &name);//добавить пользователя в таблицу
    void addMessageToBd(const QString &from, const QString &to, const QString &text);//добавить сообщения в таблицу БД
    void showUserForTable();                                          //отобразить пользователей в таблице
    void showMessageToTable();                                        //отобразить сообщения в таблице
    void changeAuthStatusOnTableUsers(const int index);               //изменить статус авторизации в таблице
    void changeAuthStatusOnTableUsersOther(const int index, bool bit);//изменить статус авторизации в таблице
    void runTcpServer();                                              //запустить TCP-сервер
    void stopTcpServer();                                             //остановить TCP-сервер
    void checkUser(const QString &login, const QString &pass);        //проверить данных пользователя, полученные по сети
    void sendComandWithOneParam(const QString &comand, const QString &param);//отправить команду с одним параметром
    void sendUsers(int size);                                         //отправить список пользователей клиенту
    void sendMessages(const QString &login);                          //отправить список сообщений клиенту
    void sendUsersAndMessages(const QString &login);                  //отправить список сообщений и пользователей клиенту


private slots:
    void on_actionExit_triggered();
    void sendToClient(std::shared_ptr<QTcpSocket*> pSocket, const QString &str);   //отправить сообщения клиенту
    void changeStatusBlocked(const int index, const QString &login, const int row);//изменен статус блокировки в таблице
    void changeStatusBanned(const int index, const QString &login, const int row); //изменен статус бана в таблице

public slots:
    virtual void slotNewConnection();                                 //слот для получения нового соединения
    void slotReadClient();                                            //слот для чтения сообщений от клиента
};
#endif // MAINWINDOW_H
