// 2022 (c) GPLv3, acetone at i2pmail.org
// Zero Storage Captcha

#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QThreadPool>

class HttpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit HttpServer(const QString& address, quint16 port, int threads, QObject *parent = nullptr);

private:
    QThreadPool *m_pool;

    void incomingConnection (qintptr handle) override;
};

#endif // HTTPSERVER_H
