// GPLv3 (c) acetone, 2022
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

    void reader();
    void writeError(const QString &text);
};

#endif // SOCKETRUNNABLE_H
