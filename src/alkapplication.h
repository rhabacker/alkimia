/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef ALKAPPLICATION_H
#define ALKAPPLICATION_H

#include <QStringList>

class AlkApplication
{
public:
    AlkApplication(int argc, char** argv);
    QStringList arguments();
    int exec();
    static void createInstance();
};

#endif // ALKAPPLICATION_H
