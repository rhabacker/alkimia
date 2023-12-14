/*
    SPDX-FileCopyrightText: 2023 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alkregexp.h"

#include <QDebug>

#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
#include <QRegExp>
class AlkRegExp::Private : public QRegExp {
public:
    Private(const QString &s, int mode)
        : QRegExp(s, (Qt::CaseSensitivity)mode)
    {}
};
#endif

AlkRegExp::AlkRegExp()
    : d(new Private(QString(), CaseSensitive))
{
}

AlkRegExp::AlkRegExp(const QString &s, Option cs)
    : d(new Private(s, (Qt::CaseSensitivity)cs))
{
}

AlkRegExp::~AlkRegExp()
{
    delete d;
}

#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
int AlkRegExp::indexIn(const QString &str, int offset) const
{
    return d->indexIn(str, offset);
}

int AlkRegExp::lastIndexOf(const QString &str, int offset) const
{
    return d->lastIndexIn(str, offset);
}

QString AlkRegExp::cap(int nth) const
{
    return d->cap(nth);
}

int AlkRegExp::pos(int nth) const
{
    return d->pos(nth);
}

void AlkRegExp::setPattern(const QString &pattern)
{
    d->setPattern(pattern);
}

void AlkRegExp::setPatternOptions(Option option)
{
    Qt::CaseSensitivity cs;
    switch(option) {
        case CaseSensitiveOption:
        case CaseSensitive:
            cs = Qt::CaseSensitive;
            break;
        case CaseInsensitiveOption:
        case CaseInsensitive:
            cs = Qt::CaseInsensitive;
            break;
    }

    d->setCaseSensitivity(cs);
}

QString AlkRegExp::remove(QString &s)
{
    return s.remove(*d);
}

QString AlkRegExp::remove(QString &s) const
{
    return s.remove(*d);
}

QString AlkRegExp::replace(QString &s, const QString &after) const
{
    return s.replace(*d, after);
}

QStringList AlkRegExp::split(const QString &s) const
{
    return s.split(*d);
}

AlkRegExpMatch AlkRegExp::match(const QString &s) const
{
    return AlkRegExpMatch(*this, s);
}

AlkRegExpMatch::AlkRegExpMatch()
    : _regex(0)
    , _pos(0)
{
}

AlkRegExpMatch::AlkRegExpMatch(const AlkRegExp &regex, const QString &s)
    : _regex(&regex)
{
    _pos = _regex->indexIn(s);
}

bool AlkRegExpMatch::hasMatch()
{
    return _pos != -1;
}

QString AlkRegExpMatch::captured(int i)
{
    return _regex->cap(i);
}
#else
#endif
