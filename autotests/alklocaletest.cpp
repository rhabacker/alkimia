/*
    SPDX-FileCopyrightText: 2025 Ralf Habacker ralf.habacker@freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: GPL-2.1-or-later
*/

#include <QtCore/QObject>

#include "alklocale.h"
#include "test.h"

class AlkLocaleTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testSetup();
    void testI18n();
};

QTEST_GUILESS_MAIN(AlkLocaleTest)

void AlkLocaleTest::testSetup()
{

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    QCOMPARE(_alk().catalogName(), QLatin1String("alkimia"));
#endif
}

void AlkLocaleTest::testI18n()
{
    QString s = "Source";
    qDebug() << s << "translated to" << i18n(s.toLatin1());
}

#include "alklocaletest.moc"
