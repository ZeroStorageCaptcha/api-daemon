// GPLv3 (c) acetone, 2022
// Zero Storage Captcha

#ifndef JSONANSWER_H
#define JSONANSWER_H

#include <QString>
#include <QJsonObject>

class JsonAnswer
{
public:
    JsonAnswer() {}

    template <class T>
    void setValue(const QString& key, T value)
    {
        m_obj.insert(key, value);
    }

    void setError(const QString& text);
    QByteArray document() const;

private:
    QJsonObject m_obj;
};

#endif // JSONANSWER_H
