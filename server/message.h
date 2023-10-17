#ifndef MESSAGE_H
#define MESSAGE_H

#include <QString>

class Message {
   QString sendedFrom_;                                 //отправитель сообщения
   QString sendedTo_;                                   //получатель сообщения
   QString text_;                                       //текст сообщения
   QString dateandtime_;                                //дата отправки сообщения

public:
   Message();
   Message(const QString &from, const QString &to, const QString &text);
   Message(const QString &from, const QString &to, const QString &text, const QString &dateandtime);
   Message(const Message& other);
   Message &operator= (const Message& other);
   ~Message() = default;

   const QString& getSendedFrom() const;                //Получить отправителя сообщения
   const QString& getSendedTo() const;                  //Получить получателя сообщения
   const QString& getText() const;                      //Получить текст сообщения
   const QString& getDatetime() const;                  //Получить дату и время
   bool checkToSendedTo(const QString &login) const;    //Соответсвует ли поле "кому отправлено сообщение" переданной строке
   bool checkToSendeFrom(const QString &login) const;   //Соответсвует ли поле "от кого" переданной строке
};

#endif // MESSAGE_H
