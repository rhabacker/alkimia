/*
    SPDX-FileCopyrightText: 2019 Ralf Habacker ralf.habacker @freenet.de
    SPDX-FileCopyrightText: 2019 Thomas Baumgart tbaumgart @kde.org

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alkpayeelinkswidget.h"

#include <QApplication>
#include <QDialog>
#include <QGridLayout>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QDialog dialog;
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(new AlkPayeeLinksWidget);
    dialog.setLayout(layout);
    dialog.show();
    app.exec();
}
