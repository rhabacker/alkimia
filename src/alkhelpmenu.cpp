/*
    SPDX-FileCopyrightText: 2025 Ralf Habacker ralf.habacker@freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "alkhelpmenu.h"

#include "alklocale.h"
#include <KAboutData>
#include <QMainWindow>
#include <QMenuBar>

static void checkAndSetVisible(QAction *action, bool state)
{
    if (action)
        action->setVisible(state);
}

AlkHelpMenu::AlkHelpMenu(QMainWindow *parent, const KAboutData &aboutData, bool showWhatsThis)
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    : KHelpMenu(parent, aboutData, showWhatsThis)
{
    QString appName = aboutData.displayName();
#else
    : KHelpMenu(parent, &aboutData, showWhatsThis)
{
    QString appName = aboutData.programName();
#endif
    parent->menuBar()->addMenu((QMenu *)menu());
    checkAndSetVisible(action(KHelpMenu::menuHelpContents), false);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    checkAndSetVisible(action(KHelpMenu::menuReportBug), false);
#endif
    checkAndSetVisible(action(KHelpMenu::menuSwitchLanguage), true);
    action(KHelpMenu::menuAboutApp)->setText(i18n("&About %1", appName));
}
