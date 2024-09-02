#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <QString>

namespace StringUtils
{
QString capitalize(QString str);
QString toTitleCase(const QString &str);
QString toSnakeCase(const QString &str);
};

#endif // STRINGUTILS_H
