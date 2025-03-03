/*
    SPDX-FileCopyrightText: 2025 Ralf Habacker ralf.habacker@freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef ALKHELPMENU_H
#define ALKHELPMENU_H

#include <KHelpMenu>

class QMainWindow;
class KAboutData;

/**
 * Wrapper for KDE provided help menu
 */
class AlkHelpMenu : public KHelpMenu
{
public:
    AlkHelpMenu(QMainWindow *parent, const KAboutData &aboutData, bool showWhatsThis = true);
};

#endif // ALKHELPMENU_H
