/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef ALKPAYEEREFERENCELINKTEST_H
#define ALKPAYEEREFERENCELINKTEST_H

#include <QtCore/QObject>

#include "alkpayeereferencelink.h"

class AlkPayeeReferenceLinkTest : public QObject
{
    Q_OBJECT

    AlkPayeeReferenceLinkData data;

private Q_SLOTS:
    void init();
    void cleanup();
    void testLink();
    void testDecorateLink();
    void testMatchingLinks();
};

#endif
