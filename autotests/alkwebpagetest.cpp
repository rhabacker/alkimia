/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alkwebpagetest.h"

#include "alkwebpage.h"
#include "alktestdefs.h"

#include "test.h"

#include <QSignalSpy>

void AlkWebPageTest::testToHtml()
{
    AlkWebPage page;

    QString content(QLatin1String("<html><head></head><body></body></html>"));
    page.setHtml(content);
    QVERIFY(page.toHtml().contains(QLatin1String("</body></html>")));
}

void AlkWebPageTest::testLoad()
{
    AlkWebPage page;

    QSignalSpy spyStarted(&page, SIGNAL(loadStarted()));
    QSignalSpy spyFinished(&page, SIGNAL(loadFinished(bool)));

    page.load(QUrl(TEST_LAUNCH_URL), QString());

    // test signals
#if defined(BUILD_WITH_WEBKIT) || defined(BUILD_WITH_WEBENGINE)
    QVERIFY(spyFinished.wait(1000));
#else
    QWARN("spying loadFinished() does not work");
#endif
    QVERIFY(spyStarted.count() >= 1);
    QCOMPARE(spyFinished.count(), 1);
    QList<QVariant> arguments = spyFinished.takeFirst();
    QVERIFY(arguments.at(0).toBool() == true);

    // test content
    QVERIFY(page.toHtml().contains(QLatin1String("</body></html>")));
}

void AlkWebPageTest::testRedirected()
{
    AlkWebPage page;

    QSignalSpy spyStarted(&page, SIGNAL(loadStarted()));
    QSignalSpy spyFinished(&page, SIGNAL(loadFinished(bool)));
    QSignalSpy spyRedirected(&page, SIGNAL(loadRedirectedTo(QUrl)));

    QString url(TEST_LAUNCH_URL);
    page.load(QUrl(url + "&redirect=1"), QString());

    // test signals
#if defined(BUILD_WITH_WEBKIT) || defined(BUILD_WITH_WEBENGINE)
    QVERIFY(spyFinished.wait(1000));
#else
    QWARN("spying loadFinished() does not work");
#endif
    QCOMPARE(spyFinished.count(), 1);
    QVERIFY(spyStarted.count() >= 1);
    QList<QVariant> arguments = spyFinished.takeFirst();
    QVERIFY(arguments.at(0).toBool() == true);
#if defined(BUILD_WITH_WEBENGINE) && QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    QCOMPARE(spyRedirected.count(), 1);
    arguments = spyRedirected.takeFirst();
    QCOMPARE(arguments.at(0).toUrl(), QUrl(url));
#else
    QWARN("This engine does not emit signal loadRedirectedTo()");
    QWARN("This engine does not return a redirected url");
#endif
    // test content
    QVERIFY(page.toHtml().contains(QLatin1String("</body></html>")));
}

QTEST_MAIN(AlkWebPageTest)
