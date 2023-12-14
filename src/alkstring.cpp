/*
    SPDX-FileCopyrightText: 2023 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alkregexp.h"
#include "alkstring.h"

#include <QDebug>


AlkString::AlkString(const QString &s)
    : QString(s)
{
}

AlkString::AlkString(const QChar &c)
    : QString(c)
{
}

AlkString::AlkString(const QLatin1Char &c)
    : QString(c)
{
}

int AlkString::indexOf(AlkRegExp &rx) const
{
    return rx.indexIn(*this);
}

int AlkString::lastIndexOf(AlkRegExp &rx, int pos) const
{
    return rx.lastIndexOf(*this, pos);
}

AlkString AlkString::remove(AlkRegExp &rx)
{
    return AlkString(rx.remove(*this));
}

AlkString AlkString::remove(const AlkRegExp &rx)
{
    return AlkString(rx.remove(*this));
}

AlkString AlkString::replace(const AlkRegExp &rx, const QString &after)
{
    return AlkString(rx.replace(*this, after));
}

QStringList AlkString::split(const AlkRegExp &rx)
{
    return rx.split(*this);
}
