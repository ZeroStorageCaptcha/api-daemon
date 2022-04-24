// 2022 (c) GPLv3, acetone at i2pmail.org
// Zero Storage Captcha

#ifndef SOCKETRUNNABLE_H
#define SOCKETRUNNABLE_H

#include <QRunnable>
#include <QTcpSocket>

class SocketRunnable : public QRunnable
{
public:
    SocketRunnable(qintptr descr) : m_socketDescriptor(descr) {}

protected:
    void run() override;

private:
    qintptr m_socketDescriptor;
    QTcpSocket* m_socket;

    QString m_request;

    void settings();
    void generate();
    void validate();

    inline QString readBeforeSpace();
    inline QString getValue(const QString& key) const;

    void wariningLog(const QString& str) const;
    void reader();
    void writeJsonError(const QString& text);
};

#endif // SOCKETRUNNABLE_H
