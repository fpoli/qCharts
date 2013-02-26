#include "exceptions.h"

MessageException::MessageException(const QString& msg) : message(msg)
{
}

MessageException::operator QString() {
    return QString(message);
}

XMLParserException::XMLParserException(const QString& msg) : MessageException(msg)
{
}

FileException::FileException(const QString& msg) : MessageException(msg)
{
}
