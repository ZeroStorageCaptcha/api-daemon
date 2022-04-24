// 2022 (c) GPLv3, acetone at i2pmail.org
// Zero Storage Captcha

#include "httpserver.h"
#include "socketrunnable.h"
#include <iostream>

HttpServer::HttpServer(const QString& address, quint16 port, int threads, QObject *parent) :
    QTcpServer(parent)
{
    m_pool = new QThreadPool(this);
    m_pool->setMaxThreadCount(threads);

    if (not listen(QHostAddress(address), port))
    {
        throw std::runtime_error("Web server not binded at " + address.toUtf8() + ":" + QString::number(port).toUtf8());
    }

    std::cout << "REST API interface at http://" << serverAddress().toString().toStdString() << ":" << serverPort() << "/\n"
                 "Threads: " << threads << std::endl;
}

void HttpServer::incomingConnection(qintptr handle)
{
    SocketRunnable *runnableSocket = new SocketRunnable(handle);
    runnableSocket->setAutoDelete(true);
    m_pool->start(runnableSocket);
}
