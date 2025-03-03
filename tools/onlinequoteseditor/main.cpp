/*
    SPDX-FileCopyrightText: 2018-2024 Ralf Habacker ralf.habacker @freenet.de
    SPDX-FileCopyrightText: 2019 Thomas Baumgart tbaumgart @kde.org

    This file is part of libalkimia.

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "mainwindow.h"

#include "alkapplication.h"
#include "alkhelpmenu.h"
#include "alklocale.h"

#include <KAboutData>

#if defined(Q_OS_WIN)
#include <QNetworkProxyFactory>
#endif

int main(int argc, char **argv)
{
    AlkApplication app(argc, argv);

    AlkAboutData about(QStringLiteral("onlinequoteseditor"),
                       "Online Quotes Editor",
                       QStringLiteral("1.0"),
                       "Editor for online price quotes used by finance applications",
                       AlkAboutData::License_GPL,
                       "(C) 2018-2024 Ralf Habacker");

#if defined(Q_OS_WIN)
    QNetworkProxyFactory::setUseSystemConfiguration(true);
#endif

    MainWindow w;
    AlkHelpMenu helpMenu(&w, about);
    w.show();
    return app.exec();
}
