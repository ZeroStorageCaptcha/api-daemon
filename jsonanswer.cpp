#include "jsonanswer.h"

#include <QJsonDocument>

void JsonAnswer::setError(const QString &text)
{
    m_obj.insert("status", false);
    m_obj.insert("message", text);
}

QByteArray JsonAnswer::document() const
{
    QByteArray objStr = QJsonDocument(m_obj).toJson(QJsonDocument::JsonFormat::Compact);
    QString result = "HTTP/1.1 200 OK\r\nContent-Type: application/json; charset=utf-8\r\n"
                     "Content-Length: "+QString::number(objStr.size())+"\r\n\r\n" + objStr;
    return result.toUtf8();
}
