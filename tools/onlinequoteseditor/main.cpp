/*
    SPDX-FileCopyrightText: 2018-2024 Ralf Habacker ralf.habacker @freenet.de
    SPDX-FileCopyrightText: 2019 Thomas Baumgart tbaumgart @kde.org

    This file is part of libalkimia.

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "alkaboutdata.h"
#include "alkapplication.h"
#include "alkhelpmenu.h"
#include "alklocale.h"
#include "mainwindow.h"

#if defined(Q_OS_WIN) && !defined(BUILD_WITH_KIO)
#include <QNetworkProxyFactory>
#endif

int main(int argc, char **argv)
{
    AlkApplication app(argc, argv);

    AlkAboutData about(QStringLiteral("onlinequoteseditor"),
                       i18n("Online Quotes Editor"),
                       QStringLiteral("1.0"),
                       i18n("Editor for online price quotes used by finance applications"),
                       LICENCE_GPL,
                       i18n("(C) 2018-2019 Ralf Habacker"));
    AlkAboutData::setApplicationData(about);

#if defined(Q_OS_WIN)
    QNetworkProxyFactory::setUseSystemConfiguration(true);
#endif

    MainWindow w;
    AlkHelpMenu helpMenu(&w, about);
    w.show();
    return app.exec();
}
