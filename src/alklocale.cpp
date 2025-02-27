/*
    SPDX-FileCopyrightText: 2025 Ralf Habacker ralf.habacker@freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "alklocale.h"

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include <kcomponentdata.h>
#include <klocale.h>

static KComponentData alk(TRANSLATION_DOMAIN);
static KLocale locale(TRANSLATION_DOMAIN);

KLocale &_locale()
{
    return locale;
}

KComponentData &_alk()
{
    return alk;
}

#endif
