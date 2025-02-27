/*
    SPDX-FileCopyrightText: 2025 Ralf Habacker ralf.habacker@freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: GPL-2.1-or-later
*/


#include "alklocale.h"
#include "test.h"

#include <QtCore/QObject>
#include <KLocalizedString>

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
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    QCOMPARE(TRANSLATION_DOMAIN, "alkimia");
    KLocalizedString::setApplicationDomain("alkimia");
    KLocalizedString::addDomainLocaleDir("de_DE", QLatin1String(CMAKE_BINARY_DIR "/locale"));
#else
    QCOMPARE(_alk().catalogName(), QLatin1String("alkimia"));
#endif
}

void AlkLocaleTest::testI18n()
{
    QString s = "Source";
    QCOMPARE(i18n(s.toLatin1()), QLatin1String("Quelle"));
    s = "File";
    QString t = i18nd("onlinequoteseditor", s.toLatin1());
    QCOMPARE(i18n(t.toLatin1()), QLatin1String("Datei"));
    qDebug() << s << "translated to" << t;
    QSKIP("Check translation support from used libraries", SkipSingle);
    // qDebug() << s << "translated to" << i18n(s.toLatin1());
}

#include "alklocaletest.moc"
