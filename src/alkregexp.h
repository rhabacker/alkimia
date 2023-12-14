/*
    SPDX-FileCopyrightText: 2023 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef ALKREGEXP_H
#define ALKREGEXP_H

#include <alkimia/alk_export.h>

#include <QStringList>

class QString;

class AlkRegExpMatch;

class ALK_NO_EXPORT AlkRegExp
{
public:
    enum Option {
        CaseInsensitive = 0,
        CaseSensitive,
        CaseSensitiveOption,
        CaseInsensitiveOption,
    };
    AlkRegExp();
    explicit AlkRegExp(const QString &s, Option mode = CaseSensitive);
    ~AlkRegExp();
    int indexIn(const QString &str, int offset = 0) const;
    int lastIndexOf(const QString &str, int offset = -1) const;
    QString cap(int nth = 0) const;
    int pos(int nth = 0) const;
    void setPattern(const QString &pattern);
    AlkRegExpMatch match(const QString &s) const;
    void setPatternOptions(Option option);
    QString remove(QString &s);
    QString remove(QString &s) const;
    QString replace(QString &s, const QString &after) const;
    QStringList split(const QString &s) const;

private:
    class Private;
    Private* d;
};

class AlkRegExpMatch {
public:
    const AlkRegExp *_regex;
    int _pos;
    AlkRegExpMatch();
    AlkRegExpMatch(const AlkRegExp &regex, const QString &s);
    bool hasMatch();
    QString captured(int i);
};


#endif
