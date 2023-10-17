#include "user.h"

//Оператор копирования присваивания
User& User::operator =(const User &other) {
    //Проверка на самоприсваивание
    if(this == &other){
        return *this;
    }

    login_ = other.login_;
    password_ = other.password_;
    name_ = other.name_;
    isAuth_ = false;
    isClosed_ = false;
    isBanned_ = false;

    return *this;
}

//Получить логин
const QString& User::getLogin() const { return login_; }

//Получить имя
QString User::getName() const { return name_; }

//Проверить логин
bool User::checkLogin(const QString& login) const { return login_ == login; }

//Проверить пароль
bool User::checkPassword(const QString &password) const { return password_ == password; }

//Установить бит авторизации
void User::setAuth() { isAuth_ = true; }

//Установить блокировку
void User::setBlock(){ isClosed_ = true; }

//Установить бан
void User::setBan(){ isBanned_ = true; }

//Сбросить блокировку
void User::resetBlock(){ isClosed_ = false; }

//Сбросить бан
void User::resetBan(){ isBanned_ = false; }

//Сбросить бит авторизации
void User::resetAuth() { isAuth_ = false; }

//Получить статус бана
bool User::getBan() const{ return isBanned_; }

//Получить статус блокировки
bool User::getBlock() const{ return isClosed_; }

//Получить статус авторизации
bool User::checkAuth() const { return isAuth_; }
