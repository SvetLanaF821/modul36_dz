#ifndef USER_H
#define USER_H

#include <QString>

class User{
    QString login_;                                     //логин
    QString password_;                                  //пароль
    QString name_;                                      //имя пользователя
    bool isAuth_;                                       //пользователь авторизирован
    bool isClosed_;                                     //пользователь заблокирован
    bool isBanned_;                                     //пользователь забанен

public:
    User() : login_("UNKNOWN"), password_("UNKNOWN"), name_("UNKNOWN"), isAuth_(false), isClosed_(false), isBanned_(false) {}
    User(const QString& login, const QString& password, const QString &name)
       : login_(login), password_(password), name_(name),isAuth_(false), isClosed_(false), isBanned_(false) {}
    User(const User &other) : login_(other.login_), password_(other.password_), name_(other.name_),isAuth_(false), isClosed_(false), isBanned_(false) {}
    User& operator =(const User &other);
    ~User() = default;

    const QString& getLogin() const;                    //Получить логин
    QString getName() const;                            //Получить имя
    bool checkLogin(const QString &login) const;        //Проверить логин
    bool checkPassword(const QString &password) const;  //Проверить пароль
    void setAuth();                                     //Установить бит авторизации
    void setBlock();                                    //Установить блокировку
    void setBan();                                      //Установить бан
    void resetBlock();                                  //Сбросить блокировку
    void resetBan();                                    //Сбросить бан
    void resetAuth();                                   //Сбросить бит авторизации
    bool getBan() const;                                //Получить статус бана
    bool getBlock() const;                              //Получить статус блокировки
    bool checkAuth() const;                             //Получить статус авторизации
};

#endif // USER_H
