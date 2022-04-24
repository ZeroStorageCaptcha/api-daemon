// 2022 (c) GPLv3, acetone at i2pmail.org
// Zero Storage Captcha

#include "httpserver.h"

#include <QApplication>
#include <iostream>

void usage()
{
    std::cout << "Zero Storage Captcha selfhosted REST API service 0.1 usage:\n\n"
                 "RUN\n"
                 "# To start QApplication without X-server (non-GUI system) should use:\n"
                 "# \"export QT_QPA_PLATFORM=offscreen\" in plain shell\n"
                 "# or\n"
                 "# \"Environment=QT_QPA_PLATFORM=offscreen\" in systemd service, [Service] section\n"
                 "  -a --address  Address to bind\n"
                 "  -p --port     Port to bind (7697 by default)\n"
                 "  -t --threads  Working threads (" << std::thread::hardware_concurrency() << " by default)\n" << std::endl;

    std::cout << "API (GET request, JSON response)\n"
                 "# Each response contains a boolean \"status\" that indicates the logic success of the operation.\n"
                 "# If !status, read \"message\" field.\n"
                 "  1. [Generate captcha]\n"
                 "     -> /generate?length= [>0] &difficulty= [0-5] &output= [base64|file] IF output=file: &filepath= [file system path]\n"
                 "     <- \"token\", IF output=base64: \"png\" (base64 encoded picture)\n"
                 "  2. [Validate captcha]\n"
                 "     -> /validate?answer= [user's answer] &token = [user's token]\n"
                 "     <- boolean \"valid\"\n"
                 "  3. [Settings]\n"
                 "  3.1 Tokens case sensitive to captcha answer:\n"
                 "     -> /settings?case_sensitive= [enable|disable]\n"
                 "  3.2 Only numbers mode:\n"
                 "     -> /settings?number_mode= [enable|disable]\n" << std::endl;

    std::cout << "2022 (c) GPLv3, acetone" << std::endl;

}

int main(int argc, char *argv[])
{
    // To start QApplication without X-server (non-GUI system) should use:
    // "export QT_QPA_PLATFORM=offscreen" in plain shell
    // or
    // "Environment=QT_QPA_PLATFORM=offscreen" in systemd service ([Service] section)
    QApplication a(argc, argv);

    int threads = std::thread::hardware_concurrency();
    quint16 port = 7697;
    QString address;

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
                throw std::invalid_argument("Port not int");
            }
        }
        else if ((param == "--threads" or param == "-t") and i+1 < argc)
        {
            bool ok;
            threads = QString(argv[i+1]).toInt(&ok);
            if (not ok or threads < 1)
            {
                throw std::invalid_argument("Threads not int or less then 1");
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

    HttpServer server(address, port, threads);

    return a.exec();
}
