/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alknewstuffengine.h"

#include "alkpayeereferencelink.h"

#include <QApplication>
#include <QObject>
#include <QtDebug>

class TestReceiver : public QObject
{
    Q_OBJECT
Q_SIGNALS:
    void finished();

public Q_SLOTS:
    void updatesAvailable(const AlkNewStuffEntryList &entries)
    {
        for (const AlkNewStuffEntry &entry : entries) {
            qDebug() << Q_FUNC_INFO << entry.name;
        }
        Q_EMIT finished();
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

    TestReceiver receiver;
    AlkNewStuffEngine engine(&receiver);
    QEventLoop loop;

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    QObject::connect(&engine, &AlkNewStuffEngine::updatesAvailable, &receiver, &TestReceiver::updatesAvailable);
    QObject::connect(&receiver, &TestReceiver::finished, &loop, &QEventLoop::quit);
#else
    QObject::connect(&engine, SIGNAL(updatesAvailable(const AlkNewStuffEntryList &)), &receiver, SLOT(updatesAvailable(const AlkNewStuffEntryList &)));
    QObject::connect(&receiver, SIGNAL(finished()), &loop, SLOT(quit()));
#endif
    engine.init(configFile);
    for (const AlkNewStuffEntry &entry : engine.installedEntries())
        qDebug() << entry.name;
    engine.checkForUpdates();

    AlkPayeeReferenceLink link("Amazon EU order Link", engine);
    qDebug() << link.data();

    loop.exec();
}

#include "alknewstuffenginetest.moc"
