/***************************************************************************
 *   Copyright 2018 Ralf Habacker <ralf.habacker@freenet.de>               *
 *   Copyright 2019 Thomas Baumgart <tbaumgart@kde.org>                    *
 *                                                                         *
 *   This file is part of libalkimia.                                      *
 *                                                                         *
 *   libalkimia is free software; you can redistribute it and/or           *
 *   modify it under the terms of the GNU General Public License           *
 *   as published by the Free Software Foundation; either version 2.1 of   *
 *   the License or (at your option) version 3 or any later version.       *
 *                                                                         *
 *   libalkimia is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>  *
 ***************************************************************************/

#include "mainwindow.h"

#include <KAboutData>
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    #include <QApplication>
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
    QApplication app(argc,argv);
#else
    KCmdLineArgs::init(argc, argv, &about);
    KApplication app(true);
#endif

    MainWindow w;

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
