// GPLv3 (c) acetone, 2022
// Zero Storage Captcha

#include "socketrunnable.h"
#include "jsonanswer.h"
#include "cpp-lib/zerostoragecaptcha.h"

#include <QRegularExpression>
#include <QFile>

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
            writeError("Invalid case_sensitive argument: expected enable or disable");
            return;
        }
        ZeroStorageCaptcha::setCaseSensitive(enable);
    }

    QString numberMode = getValue("number_mode");
    if (not numberMode.isEmpty())
    {
        bool enable;
        if (numberMode == "enable") enable = true;
        else if (numberMode == "disable") enable = false;
        else
        {
            writeError("Invalid number_mode argument: expected enable or disable");
            return;
        }
        ZeroStorageCaptcha::setNumbersOnlyMode(enable);
    }

    if (setCaseSensitive.isEmpty() and numberMode.isEmpty())
    {
        writeError("Invalid key: expected case_sensitive or number_mode");
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
    JsonAnswer answer;
    QString errorMessage;

    int length = getValue("length").toInt();
    if (length < 0)
    {
        errorMessage += "Length must be greater than 0;";
    }

    int difficulty = getValue("difficulty").toInt();
    if (difficulty < 0 or difficulty > 2)
    {
        errorMessage += "Difficulty can take values from 0 to 2;";
    }

    ZeroStorageCaptcha captcha;
    captcha.setDifficulty(difficulty);
    captcha.generateAnswer(length);
    captcha.render();

    answer.setValue("status", errorMessage.isEmpty());
    if (not errorMessage.isEmpty())
    {
        answer.setValue("message", errorMessage);
    }
    answer.setValue("png", QString(captcha.picturePng().toBase64()));
    answer.setValue("token", captcha.token());
    m_socket->write(answer.document());
}

void SocketRunnable::validate()
{
    QString token = getValue("token");
    if (token.isEmpty())
    {
        writeError("Empty token");
        return;
    }

    QString answer = getValue("answer");
    if (answer.isEmpty())
    {
        writeError("Empty answer");
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
    static QRegularExpression rgx_allFromAmpersand("&.*$");
    result.remove(rgx_allFromAmpersand);
    return result;
}

void SocketRunnable::reader()
{
    QString reqType = readBeforeSpace();

    if (reqType != "GET")
    {
        writeError("Invalid request: expected GET");
        return;
    }

    QString urlPath = readBeforeSpace();
    m_request = urlPath;
    static QRegularExpression rgx_allToQuestionMark("^.*\\?");
    m_request.remove(rgx_allToQuestionMark);
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
        writeError("Invalid request path: expected /generate, /validate or /settings");
    }
}

void SocketRunnable::writeError(const QString &text)
{
    JsonAnswer answer;
    answer.setError(text);
    m_socket->write(answer.document());
}
