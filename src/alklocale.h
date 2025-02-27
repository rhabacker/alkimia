/*
    SPDX-FileCopyrightText: 2025 Ralf Habacker ralf.habacker@freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef ALKLOCALE_H
#define ALKLOCALE_H

#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <klocalizedstring.h>
#else
#include <kcomponentdata.h>
#include <klocale.h>
#include <kglobal.h>

KLocale &_locale();
KComponentData &_alk();

inline QString _i18n(const char *text)
{
    return ki18n(text).toString(&_locale());
}

template<typename A1>
inline QString _i18n(const char *text, const A1 &a1)
{
    // STATIC_ASSERT_NOT_LITERAL_STRING(A1)
    return ki18n(text).subs(a1).toString(&_locale());
}
template<typename A1, typename A2>
inline QString _i18n(const char *text, const A1 &a1, const A2 &a2)
{
    // STATIC_ASSERT_NOT_LITERAL_STRING(A1)
    return ki18n(text).subs(a1).subs(a2).toString();
}
template<typename A1, typename A2, typename A3>
inline QString _i18n(const char *text, const A1 &a1, const A2 &a2, const A3 &a3)
{
    // STATIC_ASSERT_NOT_LITERAL_STRING(A1)
    return ki18n(text).subs(a1).subs(a2).subs(a3).toString(&_locale());
}

#define i18n _i18n

inline QString _i18nc(const char *ctxt, const char *text)
{
    return ki18nc(ctxt, text).toString(&_locale());
}

template<typename A1>
inline QString _i18nc(const char *ctxt, const char *text, const A1 &a1)
{
    return ki18nc(ctxt, text).subs(a1).toString(&_locale());
}
#define i18nc _i18nc
#endif

#endif // ALKLOCALE_H
