/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alkapplication.h"

#include "alklocale.h"

#include <KAboutData>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QApplication>
#else
#include <KApplication>
#endif

class Private
{
public:
    int _argc{0};
    char **_argv{nullptr};
    QStringList _args;
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    QApplication *_app{nullptr};
#else
    KApplication *_app{nullptr};
#endif
};

Q_GLOBAL_STATIC(Private, d)

AlkApplication::AlkApplication(int argc, char** argv)
{
    d()->_argc = argc;
    d()->_argv = argv;
    for (int i = 0; i < argc; i++)
        d()->_args.append(argv[i]);
}

QStringList AlkApplication::arguments()
{
    return d()->_args;
}

int AlkApplication::exec()
{
    return d()->_app->exec();
}

void AlkApplication::createInstance()
{
#if QT_VERSION > QT_VERSION_CHECK(5, 0, 0)
    d()->_app = new QApplication(d->_argc, d->_argv);
#else
    d()->_app = new KApplication(true);
#endif
}
