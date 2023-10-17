#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <QDataStream>
#include <QCryptographicHash>
#include <QDateTime>
#include <QMessageBox>
#include <QThread>

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
    , sd(new StartDialog(this))
    , r(new Registration(this))
    , currentLogin_("UNKNOWN")
    , currentChat_("all")
    , currentChatUser_("all")
    , host_("localhost")
    , port_(54321)
    , tcpSocket_(new QTcpSocket(this))
    , nextBlockSize_(0)
    , users_()
    , messages_()
    , isBegin_(true)
{
    ui->setupUi(this);

    connect(r.get(), SIGNAL(addUser(const QString &, const QString &, const QString &)), this, SLOT(sendUserToServer(const QString &, const QString &, const QString &)));
    connect(sd.get(), SIGNAL(sendData(const QString&, const QString&, const QString&, const QString&)), this, SLOT(checkEnterData(const QString&, const QString&, const QString&, const QString&)));
}

MainWidget::MainWidget(const QString &host, int port, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
    , sd(new StartDialog(this))
    , r(new Registration(this))
    , currentLogin_("UNKNOWN")
    , currentChat_("all")
    , currentChatUser_("all")
    , host_(host)
    , port_(port)
    , tcpSocket_(new QTcpSocket(this))
    , nextBlockSize_(0)
    , users_()
    , messages_()
    , isBegin_(true)
{
    ui->setupUi(this);

    ui->comboBox->addItem("Общий чат");
    ui->comboBox->addItem("Личный чат");

    connect(r.get(), SIGNAL(addUser(const QString &, const QString &, const QString &)), this, SLOT(sendUserToServer(const QString &, const QString &, const QString &)));
    connect(sd.get(), SIGNAL(sendData(const QString&, const QString&, const QString&, const QString&)), this, SLOT(checkEnterData(const QString&, const QString&, const QString&, const QString&)));
    connect(ui->comboBox, SIGNAL(currentTextChanged(const QString&)), this, SLOT(slotChagedComboBox(const QString&)));
    connect(ui->comboUsers, SIGNAL(currentTextChanged(const QString&)), this, SLOT(slotChagedComboUsers(const QString&)));

    ui->labelOtherLogin->setText("Общий чат");
}

MainWidget::~MainWidget()
{
    tcpSocket_->disconnectFromHost();
    tcpSocket_->close();
    delete ui;
}

//Показать окно для входа
void MainWidget::showStartMenu()
{
    sd->show();
}

//Показать окно для регистрации
void MainWidget::showStartMenuRegistration()
{
    r->show();
}

//Отправить данные пользователя на сервер
void MainWidget::sendUserDataToServer(const QString &login, const QString &pass)
{
    //Объекты для отправки
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_2);

    //Вычисляем хеш из пароля
    QByteArray ba = pass.toUtf8();
    QString cPass = QCryptographicHash::hash(ba, QCryptographicHash::Sha256).toHex();

    //Команда серверу, которая будет отправлена первой
    QString symbol = "CheckUser";

    out << quint16(0) << symbol << quint16(0) << login << quint16(0) << cPass;
    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    //Отправляем данные
    tcpSocket_->write(arrBlock);

    //Теперь текущий пользователь login
    currentLogin_ = login;
}

//Отправить запрос на получение списка пользователей и списка сообщений
void MainWidget::sendUserAndMessagesRequest()
{
    //Объекты для отправки
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_2);

    //Команда серверу
    QString symbol = "UsersAndMessages";

    //Добавляем на отправку данные
    out << quint16(0) << symbol << quint16(0) << currentLogin_;
    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    //Отправляем
    tcpSocket_->write(arrBlock);
}

//Отправить запроc на добавление сообщения на сервер
void MainWidget::sendMessageToServer(const QString &from, const QString &to, const QString &text)
{
    //Объекты для отправки
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_2);

    //Команда серверу
    QString symbol = "AddMessage";

    //Добавляем на отправку данные
    out << quint16(0) << symbol << quint16(0)<< from << quint16(0) << to << quint16(0) << text;
    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    //отправляем данные
    tcpSocket_->write(arrBlock);
}

//Отправить запрос на добавление пользователя  на сервер
void MainWidget::sendUserToServer(const QString &login, const QString &name, const QString &pass)
{
    //Присваиваем значения
    host_ = "localhost";
    port_ = 54321;

    //Соединяемся
    tcpSocket_->connectToHost(host_, port_);

    //Делаем кнопку неактивной
    r->setButtonEnabled(false);

    if(!tcpSocket_->waitForConnected(2000)){
        r->setErrorMessageReg("Ошибка соединения. Попробуйте еще раз.");
        r->setButtonEnabled(true);      //делаем кнопку опять активной
        return;
    }

    //Соединяем слоты с сигналами
    connect(tcpSocket_.get(), SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(tcpSocket_.get(), SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this, SLOT(slotError(QAbstractSocket::SocketError)));

    //Объекты для отправки
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_2);

    //Вычисляем хеш из пароля
    QByteArray ba = pass.toUtf8();
    QString cPass = QCryptographicHash::hash(ba, QCryptographicHash::Sha256).toHex();

    QString symbol = "AddUser";     //Команда для сервера

    //Добавляем все значения
    out << quint16(0) << symbol << quint16(0) << login << quint16(0) <<name << quint16(0) << cPass;
    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    //Отправляем пользователя
    tcpSocket_->write(arrBlock);

    r->setButtonEnabled(true);      //делаем кнопку опять активной
}


//Получить список сообщений и пользователей
void MainWidget::receiveUsersAndMessages(const QVector<QString> &v)
{
    int sizeUsers = v[1].toInt();
    users_.clear();

    int pos = 2;
    for(int i = 0; i < sizeUsers; ++i){
        users_.push_back(User(v[pos], v[pos + 1]));
        pos += 2;
    }

    int sizeMessages = v[pos].toInt();

     ++pos;
     messages_.clear();
     for(int i = 0; i < sizeMessages; ++i){
         messages_.push_back(Message(v[pos], v[pos+ 1], v[pos + 2], v[pos + 3]));
         pos += 4;
     }

}

//Отправить команду с одним параметром
void MainWidget::sendComandWithOneParam(const QString &comand, const QString &param)
{
    //Объекты для отправки
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_2);

    //Добавляем значения
    out << quint16(0) << comand << quint16(0) << param;
    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    //Отправляем
    tcpSocket_->write(arrBlock);
}

//Показать сообщения общего чата
void MainWidget::showPublicChat()
{
    //Текущий пользователь all
    currentChatUser_ = "all";
    //Делаем поле выбора пользователей неактивным
    ui->comboUsers->setEnabled(false);
    //Очищаем поле
    ui->textMessages->clear();

    for(auto m : messages_){
        if(m.checkToSendedTo("all")){
            QString str = "<div style='color:green;'>[" + m.getDatetime() + "]" + "   " + m.getSendedFrom()  + "<br>" + m.getText() + "<br><br></div>";
            ui->textMessages->insertHtml(str);
        }
    }
}

//Показать сообщения личного чата
void MainWidget::showSelfChat()
{
    //Делаем поле выбора пользователей активным
    ui->comboUsers->setEnabled(true);
    //Очищаем поле
    ui->textMessages->clear();

    for(auto m : messages_){
        if(m.checkToSendedTo(currentLogin_) || m.checkToSendeFrom(currentLogin_)){
            if(m.checkToSendedTo(currentChatUser_) || m.checkToSendeFrom(currentChatUser_)){
                QString str = "<div style='color:blue;'>[" + m.getDatetime() + "]" + "   " + m.getSendedFrom()  + "<br>" + m.getText() + "<br><br></div>";
                ui->textMessages->insertHtml(str);
            }
        }
    }
}

//Для обновления окна
void MainWidget::updateUsers()
{
    //Очищаем список
    ui->comboUsers->clear();
    //Проходим по всему списку и добавляем
    if(users_.size() > 0){
        for(auto u : users_){
            if(!u.checkLogin(currentLogin_)){
                ui->comboUsers->addItem(u.getLogin());
            }
        }
    }
}

//Метод, срабатываемый при закрытии окна
void MainWidget::closeEvent(QCloseEvent *event)
{
    sendComandWithOneParam("Exit", currentLogin_);
    event->accept();
}

//Отправить запрос на получение списка сообщений
void MainWidget::sendMessagesRequest()
{
    //Объекты для отправки
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_2);

    //Команда серверу
    QString symbol = "Messages";

    //Добавляем на отправку данные
    out << quint16(0) << symbol << quint16(0) << currentLogin_;
    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    //Отправляем
    tcpSocket_->write(arrBlock);
}

//Получить список сообщений
void MainWidget::receiveMessages(const QVector<QString> &v)
{
     int size = v[1].toInt();
     messages_.clear();
     for(int i = 0; i < size; i+=4){
         messages_.push_back(Message(v[i], v[i+ 1], v[i + 2], v[i + 3]));
     }
}


//Cлот для проверки входных данных
void MainWidget::checkEnterData(const QString &login, const QString &password, const QString &host, const QString &port)
{
    //Присваиваем значения
    host_ = host;
    port_ = port.toInt();

    tcpSocket_->connectToHost(host_, port_);//Соединяемся
    sd->setButtonEnabled(false);            //Делаем кнопку неактивной

    if(!tcpSocket_->waitForConnected(2000)){
        sd->setErrorMessage("Ошибка соединения. Попробуйте еще раз.");
        sd->setButtonEnabled(true);         //Делаем кнопку опять активной
        return;
    }

    //Соединяем слоты с сигналами
    connect(tcpSocket_.get(), SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(tcpSocket_.get(), SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this, SLOT(slotError(QAbstractSocket::SocketError)));

    sendUserDataToServer(login, password);  //Отправляем данные
    currentLogin_ = login;                  //Присваиваем значение
    sd->setButtonEnabled(true);             //Делаем кнопку опять активной
}

//Слот для чтения данных от сервера
void MainWidget::slotReadyRead()
{
    QDataStream in(tcpSocket_.get());
    in.setVersion(QDataStream::Qt_5_3);
    QVector<QString> v;                     //вектор для получения сообщений
    for(;;){
        if(!nextBlockSize_){
            if(tcpSocket_->bytesAvailable() < qint64(sizeof(quint16))){
                break;
            }
            in >> nextBlockSize_;
        }

        if(tcpSocket_->bytesAvailable() < nextBlockSize_){
            break;
        }

        QString str;
        in >> str;

        v.push_back(str);
        nextBlockSize_ = 0;
    }

    if(v.size() > 0){
        if(v[0] == "AddUser"){
            //Получаем результат добавления пользователя
            if(v[1] == "Yes"){
                r->hide();
                tcpSocket_->disconnectFromHost();
                tcpSocket_->close();
            }
            if(v[1] == "No"){
                r->setErrorMessageReg("Такой пользователь уже зарегистрирован");
                r->clearFields();
                tcpSocket_->disconnectFromHost();
                tcpSocket_->close();
            }
        }
        else if(v[0] == "CheckUser"){
            //Получаем результат проверки пользователя
            if(v[1] == "Yes"){
                sendUserAndMessagesRequest();
                sd->hide();
                ui->labelSelfLogin->setText("Добро пожаловать, " + currentLogin_);
            }
            else if(v[1] == "Ban"){
                sendUserAndMessagesRequest();
                sd->hide();
                ui->labelSelfLogin->setText("Добро пожаловать, " + currentLogin_);
                ui->buttonSend->setEnabled(false);
            }
            else if(v[1] == "Block"){
                sd->setErrorMessage("User is blocked");
                currentLogin_ = "UNKNOWN";
                tcpSocket_->disconnectFromHost();
                tcpSocket_->close();
            }
            else{
                sd->setErrorMessage("Логин или пароль введены неверно");
                currentLogin_ = "UNKNOWN";
                tcpSocket_->disconnectFromHost();
                tcpSocket_->close();
            }
        }
        else if(v[0] == "UsersAndMessages"){
            //Получаем список пользователей
            receiveUsersAndMessages(v);
            QThread::msleep(500);
            updateUsers();
            showPublicChat();
        }
        else if(v[0] == "Blocked"){
            QMessageBox::information(this, "Внимание", "Вы заблокированы");
            this->close();
        }
        else if(v[0] == "Banned"){
            ui->buttonSend->setEnabled(false);
            QMessageBox::information(this, "Внимание", "Вы забанены. Вы не можете отправлять сообщения");
        }
        else if(v[0] == "UnBanned"){
            ui->buttonSend->setEnabled(true);
            QMessageBox::information(this, "Внимание", "Вы разбанены. Можете отправлять сообщения");
        }
        else if(v[0] == "Messages"){
            receiveMessages(v);
        }
    }
}

//Ошибки с сетью
void MainWidget::slotError(QAbstractSocket::SocketError err)
{
    QString strError = "Error: " + (err == QAbstractSocket::HostNotFoundError ?
                                           "The host was not found." :
                                           err == QAbstractSocket::RemoteHostClosedError ?
                                               "Remote host is closed." :
                                               err == QAbstractSocket::ConnectionRefusedError ?
                                                   "The connection was refused." :
                                                   QString(tcpSocket_->errorString()));
    qDebug() << strError;
}

//Слот для обработки выбора ComboBox с чатами
void MainWidget::slotChagedComboBox(const QString &text)
{
    ui->textMessages->clear();      //очищаем поле
    currentChat_ = text;

    if(text == "Общий чат"){
        currentChatUser_ = "all";
        ui->labelOtherLogin->setText("Общий чат");
        showPublicChat();
    }
    else{
        currentChatUser_ = ui->comboUsers->currentText();
        ui->labelOtherLogin->setText("Личный чат");
        showSelfChat();
    }
}

//Слот для обработки выбора ComboBox с выбором пользователя
void MainWidget::slotChagedComboUsers(const QString &text)
{
    currentChatUser_ = text;

   if(isBegin_){
       isBegin_ = false;
       return;
   }

   showSelfChat();
}

//Слот для обработки кнопки отправки сообщения
void MainWidget::on_buttonSend_clicked()
{
    //Считываем поле
    QString text = ui->lineEditMessage->text();

    //Устанавливаем границы по умолчанию
    ui->lineEditMessage->setStyleSheet("border:0px;");
    ui->lineEditMessage->setStyleSheet("border-bottom:1px solid black;");

    //Если поле пустое
    if(text.isEmpty()){
        //Устанавливаем красную границу
        ui->lineEditMessage->setStyleSheet("border:1px solid red;");
        //выходим
        return;
    }

    //Добавляем сообщение в вектор
    messages_.push_back(Message(currentLogin_, currentChatUser_, text, QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss")));
    //Отправляем сообщение
    sendMessageToServer(currentLogin_, currentChatUser_, text);
    //Очищаем поле
    ui->lineEditMessage->clear();

    //Отображаем нужный чат
    if(currentChatUser_ == "all"){
        showPublicChat();
    }
    else{
        showSelfChat();
    }
}
