#ifndef USER_H
#define USER_H

#include <QString>

class User{
   QString login_;                                  //логин пользователя
   QString name_;                                   //имя пользователя

public:

   User() : login_("UNKNOWN"), name_("UNKNOWN") {}
   User(const QString& login, const QString &name)
      : login_(login), name_(name) {}
   User(const User &other) : login_(other.login_), name_(other.name_) {}
   User& operator =(const User &other);
   ~User() = default;

   bool checkLogin(const QString &login) const;     //Проверить логин
   const QString& getLogin() const;                 //Получить логин
   QString getName() const;                         //Получить имя
};

#endif // USER_H
