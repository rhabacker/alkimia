/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alkpayeereferencelinktest.h"

#include "alkpayeereferencelink.h"
#include "test.h"

QTEST_GUILESS_MAIN(AlkPayeeReferenceLinkTest)

void AlkPayeeReferenceLinkTest::init()
{
    data = {"Ebay",
            "\\d+-\\d+-\\d+",
            "https://order.ebay.de/ord/show?orderId=%1#/",
            "156-2765-3721",
            "",
            QUuid::createUuid().toString(),
            QDate::currentDate()};
}

void AlkPayeeReferenceLinkTest::cleanup()
{
}

void AlkPayeeReferenceLinkTest::testLink()
{
    AlkPayeeReferenceLink link(data);
    QCOMPARE(link.link("Teststring 123-456-789 suffix").toString(), QLatin1String("https://order.ebay.de/ord/show?orderId=123-456-789#/"));
}

void AlkPayeeReferenceLinkTest::testDecorateLink()
{
    AlkPayeeReferenceLink link(data);
    QString result = link.decorateLink("Teststring 123-456-789 suffix");
    QCOMPARE(result, QLatin1String("Teststring <123-456-789> suffix"));
}

void AlkPayeeReferenceLinkTest::testMatchingLinks()
{
    AlkPayeeReferenceLink link(data);
    QStringList reference;
    reference << "123-456-789";
    QStringList result = link.matchingLinks("Teststring 123-456-789 suffix");
    QCOMPARE(result, reference);
}
