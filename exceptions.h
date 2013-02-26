#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <QtCore>

class MessageException {
private:
    QString message;
protected:
    MessageException(const QString& msg = "");
public:
    operator QString();
};

class XMLParserException: public MessageException {
public:
    XMLParserException(const QString& msg = "");
};

class FileException: public MessageException {
public:
    FileException(const QString& msg = "");
};

#endif // EXCEPTIONS_H
