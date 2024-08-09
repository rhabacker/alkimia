/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alknewstuffengine.h"

#include "alkdebug.h"
#include "alkpayeereferencelink.h"

#include <QApplication>
#include <QObject>
#include <QTimer>

class TestReceiver : public QObject
{
    Q_OBJECT
public:
    QEventLoop &_loop;

    TestReceiver(QEventLoop &loop)
        : _loop(loop)
    {
        QTimer::singleShot(10000, this, SLOT(quitWithError()));
        connect(this, SIGNAL(finished()), this, SLOT(quit()));
    }

Q_SIGNALS:
    void finished();

public Q_SLOTS:
    void updatesAvailable(const AlkNewStuffEntryList &entries)
    {
        for (const AlkNewStuffEntry &entry : entries) {
            alkDebug() << entry.name;
        }
        Q_EMIT finished();
    }

    void quitWithError()
    {
        alkDebug() << "Timeout received - exit with error";
        _loop.exit(1);
    }

    void quit()
    {
        _loop.exit(0);
    }
};

QDebug operator<<(QDebug d, const AlkPayeeReferenceLinkData &v)
{
    d << "AlkPayeeReferenceLinkData:"
      << v.date
      << v.idPattern
      << v.name
      << v.urlTemplate
      << v.uuid;
    return d;
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QString key = app.arguments().size() == 2 ? app.arguments().at(1) : "quotes";
    QString configFile = QString("%1/alkimia-%2.knsrc").arg(KNSRC_DIR, key);

    QEventLoop loop;
    TestReceiver receiver(loop);
    AlkNewStuffEngine engine(&receiver);

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    QObject::connect(&engine, &AlkNewStuffEngine::updatesAvailable, &receiver, &TestReceiver::updatesAvailable);
#else
    QObject::connect(&engine, SIGNAL(updatesAvailable(const AlkNewStuffEntryList &)), &receiver, SLOT(updatesAvailable(const AlkNewStuffEntryList &)));
#endif
    engine.init(configFile);
    for (const AlkNewStuffEntry &entry : engine.installedEntries())
        alkDebug() << entry.name;
    engine.checkForUpdates();

    AlkPayeeReferenceLink link("Amazon EU order Link", engine);
    alkDebug() << link.data();

    return loop.exec();
}

#include "alknewstuffenginetest.moc"
