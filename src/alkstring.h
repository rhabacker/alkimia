#ifndef ALKSTRING_H
#define ALKSTRING_H

#include <QString>

class AlkRegExp;

class AlkString : public QString
{
public:
    AlkString(const QString &s);
    AlkString(const QChar &c);
    AlkString(const QLatin1Char &c);

    int indexOf(AlkRegExp &rx) const;
    int lastIndexOf(AlkRegExp &rx, int pos = 0) const;
    int indexOf(const QChar &c) { return QString::indexOf(c); }
    AlkString remove(AlkRegExp &rx);
    AlkString remove(const AlkRegExp &rx);
    AlkString remove(int i, int len) { return QString::remove(i, len); }
    AlkString replace(const AlkRegExp &rx, const QString &after);
    QStringList split(const AlkRegExp &rx);
};

#endif // ALKSTRING_H
