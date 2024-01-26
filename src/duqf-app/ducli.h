#ifndef DUCLI_H
#define DUCLI_H

#include <QCommandLineParser>

class DuCLI
{
public:
    static void initParser(QCommandLineParser *parser);

private:
    DuCLI();
};

#endif // DUCLI_H
