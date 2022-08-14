// GPLv3 (c) acetone, 2022
// Zero Storage Captcha

#include "httpserver.h"

#include <QApplication>
#include <iostream>

const std::string COPYRIGHT = "GPLv3 (c) acetone, 2022";

void usage()
{
    std::cout << "Zero Storage Captcha selfhosted REST API service 0.2 usage:\n\n"
                 "RUN\n"
                 "  -a --address  Address to bind (127.0.0.1 by default)\n"
                 "  -p --port     Port to bind (7697 by default)\n"
                 "  -t --threads  Working threads (hardware count by default)\n" << std::endl;

    std::cout << "REST API (GET request)\n"
                 "Each response contains a boolean \"status\" that indicates the logic success of the operation.\n"
                 "If !status, read \"message\" field.\n"
                 "  1. [Generate captcha]\n"
                 "     -> /generate?length=CAPTCHA_TEXT_LENGTH&difficulty=0|1|2\n"
                 "     <- { \"token\": \"CAPTCHA_TOKEN\", \"png\": \"BASE64_ENCODED_PICTURE\" }\n"
                 "  2. [Validate captcha]\n"
                 "     -> /validate?answer=CAPTCHA_ANSWER&token=CAPTCHA_TOKEN\n"
                 "     <- { \"valid\": true|false }\n"
                 "  3. [Global settings]\n"
                 "  3.1 Tokens case sensitive to captcha answer (disabled by default):\n"
                 "     -> /settings?case_sensitive=enable|disable\n"
                 "  3.2 Numbers only mode (disabled by default):\n"
                 "     -> /settings?number_mode=enable|disable\n" << std::endl;

    std::cout << COPYRIGHT << std::endl;
}

int main(int argc, char *argv[])
{
    // To start QApplication without X-server (non-GUI system) should use:
    // "export QT_QPA_PLATFORM=offscreen" in plain shell
    // or
    // "Environment=QT_QPA_PLATFORM=offscreen" in systemd service ([Service] section)
    QApplication a(argc, argv);

    int threads = static_cast<int>(std::thread::hardware_concurrency());
    quint16 port = 7697;
    QString address = "127.0.0.1";

    for (int i = 1; i < argc; i++)
    {
        QString param(argv[i]);
        if ((param == "--address" or param == "-a") and i+1 < argc)
        {
            address = argv[i+1];
        }
        else if ((param == "--port" or param == "-p") and i+1 < argc)
        {
            bool ok;
            port = QString(argv[i+1]).toUShort(&ok);
            if (not ok)
            {
                throw std::invalid_argument("Port not a number");
            }
        }
        else if ((param == "--threads" or param == "-t") and i+1 < argc)
        {
            bool ok;
            threads = QString(argv[i+1]).toInt(&ok);
            if (not ok or threads < 1)
            {
                throw std::invalid_argument("Threads not a number or less then 1");
            }
        }
        else if (param == "--help" or param == "-h")
        {
            usage();
            return 0;
        }
    }

    if (address.isEmpty() or not port)
    {
        qInfo() << "ERROR: Invalid address\n";
        usage();
        return 1;
    }

    std::cout <<
        " ______     ______     ______        ______     ______   __\n"
        "/\\___  \\   /\\  ___\\   /\\  ___\\      /\\  __ \\   /\\  == \\ /\\ \\\n"
        "\\/_/  /__  \\ \\___  \\  \\ \\ \\____     \\ \\  __ \\  \\ \\  _-/ \\ \\ \\\n"
        "  /\\_____\\  \\/\\_____\\  \\ \\_____\\     \\ \\_\\ \\_\\  \\ \\_\\    \\ \\_\\ \n"
        "  \\/_____/   \\/_____/   \\/_____/      \\/_/\\/_/   \\/_/     \\/_/\n"
        "https://github.com/ZeroStorageCaptcha/api-daemon\n"
        "https://notabug.org/ZeroStorageCaptcha/api-daemon\n"
        "http://git.i2p/ZeroStorageCaptcha/api-daemon\n"
        << COPYRIGHT << "\n\n";

    HttpServer server(address, port, threads);

    return a.exec();
}
