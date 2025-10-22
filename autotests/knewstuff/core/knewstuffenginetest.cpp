/*
    This file is part of KNewStuff2.
    SPDX-FileCopyrightText: 2008 Jeremy Whiting <jpwhiting@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include <QSignalSpy>
#include <QTest>
#include <QtGlobal>

#include <knscore/engine.h>
#include <knscore/entryinternal.h>

using namespace KNSCore;

class EngineTest : public QObject
{
    Q_OBJECT
public:
    Engine *engine = nullptr;
    const QString dataDir = QStringLiteral(DATA_DIR);

private Q_SLOTS:
    void initTestCase();
    void testPropertiesReading();
    void testProviderFileLoading();
    void testProviderUpdate();
};

void EngineTest::initTestCase()
{
    engine = new Engine(this);
    QVERIFY(engine->init(dataDir + "enginetest.knsrc"));
    QCOMPARE(engine->busyState(), Engine::BusyOperation::LoadingData);
    QSignalSpy providersLoaded(engine, &Engine::signalProvidersLoaded);
    QVERIFY(providersLoaded.wait());
    QCOMPARE(engine->busyState(), Engine::BusyState());
}

void EngineTest::testPropertiesReading()
{
    QCOMPARE(engine->name(), QStringLiteral("InstallCommands"));
    QCOMPARE(engine->categories(), QStringList({"KDE Wallpaper 1920x1200", "KDE Wallpaper 1600x1200"}));
    QCOMPARE(engine->useLabel(), QStringLiteral("UseLabelTest"));
    QVERIFY(engine->hasAdoptionCommand());
}

void EngineTest::testProviderFileLoading()
{
    const QString providerId = QUrl::fromLocalFile(dataDir + "entry.xml").toString();
    QSharedPointer<Provider> provider = engine->provider(providerId);
    QVERIFY(provider);
    QCOMPARE(engine->defaultProvider(), provider);

    KNSCore::EntryInternal::List list;
    connect(
        engine,
        &Engine::signalEntriesLoaded,
        this,
        [&list](const KNSCore::EntryInternal::List &loaded) {
            list = loaded;
        },
        Qt::DirectConnection);

    engine->setSearchTerm(QStringLiteral("Entry 4"));
    QSignalSpy spy(engine, &Engine::signalEntriesLoaded);
    QVERIFY(spy.wait());
    QCOMPARE(list.size(), 1);
    QCOMPARE(list.constFirst().name(), QStringLiteral("Entry 4 (ghns included)"));
}

void EngineTest::testProviderUpdate()
{
    Engine *engine = new Engine(this);
    QVERIFY(engine->init(dataDir + "enginetest.knsrc"));

    KNSCore::EntryInternal::List list;
    connect(
        engine,
        &Engine::signalEntriesLoaded,
        this,
        [&list](const KNSCore::EntryInternal::List &loaded) {
            list = loaded;
        },
        Qt::DirectConnection);

    engine->setSearchTerm(QStringLiteral("Entry 4"));
    QSignalSpy spy(engine, &Engine::signalEntriesLoaded);
    spy.wait();

    QSignalSpy spyInstall(engine, &Engine::busyStateChanged);
    engine->install(list.constFirst());
    QVERIFY(spyInstall.wait());
    list.clear();

    qDebug() << "update";

    const QString updateDataDir = dataDir + "update/";
    Engine *engine2 = new Engine(this);
    QVERIFY(engine2->init(updateDataDir + "enginetest.knsrc"));

    connect(
        engine2,
        &Engine::signalEntriesLoaded,
        this,
        [&list](const KNSCore::EntryInternal::List &loaded) {
            list = loaded;
        },
        Qt::DirectConnection);

    connect(
        engine2,
        &Engine::signalUpdateableEntriesLoaded,
        this,
        [&list](const KNSCore::EntryInternal::List &loaded) {
            list = loaded;
        },
        Qt::DirectConnection);

    QCOMPARE(engine2->busyState(), Engine::BusyOperation::LoadingData);
    QSignalSpy providersLoaded(engine2, &Engine::signalProvidersLoaded);
    QVERIFY(providersLoaded.wait());
    QCOMPARE(engine2->busyState(), Engine::BusyState());

    QSignalSpy spy2(engine2, &Engine::signalEntriesLoaded);
    engine2->checkForInstalled();
    QVERIFY(spy2.wait());

    //engine->setSearchTerm(QStringLiteral("Entry 4"));
    QSignalSpy spyUpdates(engine2, &Engine::signalUpdateableEntriesLoaded);

    engine2->checkForUpdates();

    QVERIFY(spyUpdates.wait());
    qDebug() << "1";
    QCOMPARE(list.size(), 1);
    QCOMPARE(list.constFirst().name(), QStringLiteral("Entry 4 (ghns included)"));
}

QTEST_MAIN(EngineTest)

#include "knewstuffenginetest.moc"
