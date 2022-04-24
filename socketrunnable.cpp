// 2022 (c) GPLv3, acetone at i2pmail.org
// Zero Storage Captcha

#include "socketrunnable.h"
#include "jsonanswer.h"
#include "cpp-lib/zerostoragecaptcha.h"

#include <QRegularExpression>
#include <QFile>
#include <QDateTime>

constexpr int URL_PATH_MAX_LENGTH {200};

void SocketRunnable::run()
{
    if(!m_socketDescriptor) return;
    m_socket = new QTcpSocket;
    m_socket->setSocketDescriptor(m_socketDescriptor);
    if (m_socket->waitForReadyRead())
    {
        reader();
    }
    while (m_socket->bytesToWrite() > 0)
    {
        if (m_socket->state() == QTcpSocket::UnconnectedState) break;
        m_socket->waitForBytesWritten();
    }
    m_socket->close();
    m_socket->deleteLater();
}

void SocketRunnable::settings()
{
    QString setCaseSensitive = getValue("case_sensitive");
    if (not setCaseSensitive.isEmpty())
    {
        bool enable;
        if (setCaseSensitive == "enable") enable = true;
        else if (setCaseSensitive == "disable") enable = false;
        else
        {
            writeJsonError("Invalid case_sensitive argument: expected enable or disable");
            return;
        }
        ZeroStorageCaptchaCrypto::KeyHolder::setCaseSensitive(enable);
    }

    QString numberMode = getValue("number_mode");
    if (not numberMode.isEmpty())
    {
        bool enable;
        if (numberMode == "enable") enable = true;
        else if (numberMode == "disable") enable = false;
        else
        {
            writeJsonError("Invalid number_mode argument: expected enable or disable");
            return;
        }
        ZeroStorageCaptcha::setOnlyNumbersMode(enable);
    }

    if (setCaseSensitive.isEmpty() and numberMode.isEmpty())
    {
        writeJsonError("Invalid key: expected case_sensitive or number_mode");
    }
    else
    {
        JsonAnswer result;
        result.setValue("status", true);
        m_socket->write(result.document());
    }
}

void SocketRunnable::generate()
{
    bool success = false;

    int length = getValue("length").toInt(&success);
    if (not success)
    {
        writeJsonError("Invalid length: expected int > 0");
        return;
    }

    int difficulty = getValue("difficulty").toInt(&success);
    if (not success)
    {
        writeJsonError("Invalid difficulty: expected int (0, 5)");
        return;
    }
    if (not success)
    {
        writeJsonError("Invalid difficulty: expected int (0, 5)");
        return;
    }

    auto res = ZeroStorageCaptcha::getCaptcha(length, difficulty);
    if (res.token().isEmpty())
    {
        writeJsonError("Temporary issue: token cache is full");
        return;
    }

    JsonAnswer result;
    QString output = getValue("output");
    if (output == "file")
    {
        QString filepath = getValue("filepath");
        filepath = QByteArray::fromPercentEncoding(filepath.toUtf8());
        filepath.replace('+', ' ');
        filepath.replace("\\", "/");
        if (filepath.isEmpty())
        {
            writeJsonError("Accepted flag to save picture to disk, but \"filepath\" is empty");
            return;
        }
        QFile f(filepath);
        if (not f.open(QIODevice::WriteOnly))
        {
            writeJsonError("Can not write file " + filepath);
            return;
        }
        f.write(res.picture());
        f.close();
    }
    else if (output == "base64")
    {
        result.setValue("png", QString(res.picture().toBase64()));
    }
    else
    {
        writeJsonError("Invalid output type: expected file or base64");
        return;
    }

    result.setValue("status", true);
    result.setValue("token", res.token());
    m_socket->write(result.document());
}

void SocketRunnable::validate()
{
    QString token = getValue("token");
    if (token.isEmpty())
    {
        writeJsonError("Empty token");
        return;
    }

    QString answer = getValue("answer");
    if (answer.isEmpty())
    {
        writeJsonError("Empty answer");
    }

    bool valid = ZeroStorageCaptcha::validate(answer, token);
    JsonAnswer result;
    result.setValue("status", true);
    result.setValue("valid", valid);
    m_socket->write(result.document());
}

QString SocketRunnable::readBeforeSpace()
{
    QString result;
    int counter = 0;
    char symbol {0};
    qint64 tail = m_socket->read(&symbol, 1);
    while (symbol != ' ' and tail > 0)
    {
        if (counter > URL_PATH_MAX_LENGTH)
        {
            wariningLog ("SocketRunnable::readBeforeSpace() size > " + QString::number(URL_PATH_MAX_LENGTH) + ": " + result);
            break;
        }
        result += symbol;
        tail = m_socket->read(&symbol, 1);
        counter++;
    }
    return result;
}

QString SocketRunnable::getValue(const QString &key) const
{
    QString result = m_request;
    QString pattern = key+"=";
    if (not result.contains(pattern)) return QString();

    result.remove(QRegularExpression("^.*"+QRegularExpression::escape(pattern)));
    if (result.isEmpty() or result.startsWith("&")) return QString();
    result.remove(QRegularExpression("&.*$"));
    return result;
}

void SocketRunnable::wariningLog(const QString &str) const
{
    qInfo().noquote() <<
        "<warning time=\"" + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")+ "\">\n"
        "  " + str + "\n"
        "</warning>";
}

void SocketRunnable::reader()
{
    QString reqType = readBeforeSpace();

    if (reqType != "GET")
    {
        writeJsonError("Invalid request: expected GET");
        m_socket->waitForBytesWritten();
        return;
    }

    QString urlPath = readBeforeSpace();
    m_request = urlPath;
    m_request.remove(QRegularExpression("^.*\\?"));
    m_request.remove('\r');
    m_request.remove('\n');

    if (urlPath.startsWith("/generate"))
    {
        generate();
    }
    else if (urlPath.startsWith("/validate"))
    {
        validate();
    }
    else if (urlPath.startsWith("/settings"))
    {
        settings();
    }
    else
    {
        writeJsonError("Invalid request path: expected /generate, /validate or /settings");
    }
}


void SocketRunnable::writeJsonError(const QString &text)
{
    JsonAnswer answer;
    answer.setError(text);
    m_socket->write(answer.document());
}
