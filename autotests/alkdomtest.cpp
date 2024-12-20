/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alkdomtest.h"

#include "alkdom.h"
#include "test.h"

QTEST_GUILESS_MAIN(AlkDomTest)

void AlkDomTest::init()
{
}

void AlkDomTest::cleanup()
{
}

void AlkDomTest::testCreateDocument()
{
    AlkDomDocument doc;

    AlkDomElement el = doc.createElement("Tag");
    el.setComment("comment for 'Tag' element");
    el.setAttribute("key", "value");
    AlkDomElement el2 = doc.createElement("SubTag");
    el2.setComment("comment for 'SubTag' element\nsecond line comment");
    el2.setAttribute("subkey", "value");
    el.appendChild(el2);
    doc.appendChild(el);

    QCOMPARE(doc.toString(false),
             QLatin1String("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<!-- comment for 'Tag' element -->\n<Tag key=\"value\">\n<!--\n    comment for 'SubTag' element\n    "
             "second line comment\n-->\n<SubTag subkey=\"value\" />\n</Tag>\n"));
    QCOMPARE(doc.toString(true),
             QLatin1String("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n <!-- comment for 'Tag' element -->\n <Tag key=\"value\">\n"
             "  <!--\n      comment for 'SubTag' element\n      second line comment\n  -->\n  <SubTag subkey=\"value\" />\n </Tag>\n"));
}
