/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alknewstuffengine.h"

#include <QApplication>
#include <QObject>
#include <QThread>
#include <QTimer>
#include "alkdebug.h"
#include <QtTest/QSignalSpy>

class TestReceiver : public QObject
{
    Q_OBJECT

public:
    AlkNewStuffEngine *engine;
    AlkNewStuffEntryList availableEntries;

Q_SIGNALS:
    void finished();

public Q_SLOTS:
    void updatesAvailable(const AlkNewStuffEntryList &entries)
    {
        for (const AlkNewStuffEntry &entry : entries) {
            alkDebug() << entry.name;
        }
    }

    void entriesAvailable(const AlkNewStuffEntryList &entries)
    {
        availableEntries = entries;
        for (const AlkNewStuffEntry &entry : entries) {
            alkDebug() << entry.name;
        }

        QTimer::singleShot(5000, this, &TestReceiver::install);
    }

    void install()
    {
        alkDebug() << "installing" << availableEntries.at(0).name;
        engine->install(availableEntries.at(0), true);
        QTimer::singleShot(5000, this, &TestReceiver::uninstall);
    }

    void uninstall()
    {
        alkDebug() << "uninstalling" << availableEntries.at(0).name;
        engine->uninstall(availableEntries.at(0));
        QTimer::singleShot(5000, this, &TestReceiver::finished);
    }
};

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QString configFile = QString("%1/%2").arg(KNSRC_DIR, "alkimiatest-quotes.knsrc");

    TestReceiver receiver;
    AlkNewStuffEngine engine(&receiver);
    receiver.engine = &engine;
    QEventLoop loop;

    QObject::connect(&engine, SIGNAL(updatesAvailable(AlkNewStuffEntryList)), &receiver, SLOT(updatesAvailable(AlkNewStuffEntryList)));
    QObject::connect(&engine, SIGNAL(entriesAvailable(AlkNewStuffEntryList)), &receiver, SLOT(entriesAvailable(AlkNewStuffEntryList)));
    QObject::connect(&receiver, SIGNAL(finished()), &loop, SLOT(quit()));

    engine.init(configFile);
    //QSignalSpy spy(&engine, SIGNAL(installFinished(AlkEntry)));

    engine.checkForUpdates();

    loop.exec();
}

#include "alknewstuffenginetest.moc"
