/*
    SPDX-FileCopyrightText: 2018 Ralf Habacker ralf.habacker @freenet.de
    SPDX-FileCopyrightText: 2019 Thomas Baumgart tbaumgart @kde.org

    This file is part of libalkimia.

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "mainwindow.h"

#include <KAboutData>
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    #include <QApplication>
    #include <QCommandLineParser>
    #include <KLocalizedString>

    #define _i18n i18n
    #define LICENCE_GPL KAboutLicense::GPL
    #define CATALOG
    #define aboutName() about.displayName()
#else
    #include <KApplication>
    #include <KCmdLineArgs>

    #undef QStringLiteral
    #define QStringLiteral QByteArray
    #define _i18n ki18n
    #define LICENCE_GPL KAboutData::License_GPL
    #define CATALOG QByteArray("onlinequoteseditor"),
    #define aboutName() about.programName()
#endif

#include <KHelpMenu>
#include <QMenuBar>

int main(int argc, char **argv)
{
    KAboutData about(QStringLiteral("onlinequoteseditor"),
                     CATALOG
                     _i18n("Online Quotes Editor"),
                     QStringLiteral("1.0"),
                     _i18n("Editor for online price quotes used by finance applications"),
                     LICENCE_GPL,
                     _i18n("(C) 2018-2019 Ralf Habacker"));

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    QStringList profiles;
    profiles << "no-config-file" << "alkimia5" << "skrooge5" << "kmymoney5";
#ifdef ENABLE_FINANCEQUOTE
    profiles << "Finance::Quote";
#endif
    QApplication app(argc,argv);
    QCommandLineParser parser;
    QCommandLineOption profileOption(QStringList() << "p" << "profile",
                                     i18n("Select profile"), i18n("profile"));
    parser.addOption(profileOption);
    parser.process(app);
    if (!parser.value(profileOption).isEmpty())
        profiles = QStringList() << parser.value(profileOption);
#else
    QStringList profiles;
    profiles << "no-config-file" << "alkimia4" << "skrooge4" << "kmymoney4";
#ifdef ENABLE_FINANCEQUOTE
    profiles << "Finance::Quote";
#endif
    KCmdLineArgs::init(argc, argv, &about);
    KApplication app(true);
    KCmdLineOptions options;
    options.add("p").add("profile \\<profile>", ki18n("Select profile"));
    KCmdLineArgs::addCmdLineOptions(options);
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    if (args->isSet("profile"))
        profiles = QStringList() << args->getOption("profile");
#endif

    MainWindow w(profiles);

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    KHelpMenu helpMenu(&w, about.shortDescription());
    QString appName = about.displayName();
#else
    KHelpMenu helpMenu(&w, &about, false);
    QString appName = about.programName();
#endif
    helpMenu.menu();
    helpMenu.action(KHelpMenu::menuHelpContents)->setVisible(false);
    helpMenu.action(KHelpMenu::menuReportBug)->setVisible(false);
    helpMenu.action(KHelpMenu::menuSwitchLanguage)->setVisible(true);
    helpMenu.action(KHelpMenu::menuAboutApp)->setText(i18n("&About %1", appName));
    w.menuBar()->addMenu((QMenu*)helpMenu.menu());

    w.show();
    return app.exec();
}
