/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alknewstuffengine.h"

#include "alkdebug.h"

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    #include <KNSCore/EngineBase>
    #include <KNSCore/Provider>
    #include <KNSCore/ResultsStream>
#elif QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    #include <knscore/engine.h>
    #include <knewstuff_version.h>
#else
    #include <knewstuff3/downloadmanager.h>
    #define KNEWSTUFF_VERSION 0
#endif

#include <QEventLoop>
#include <QPointer>
#include <QWidget>

class AlkNewStuffEngine::Private : public QObject
{
    Q_OBJECT
public:
    AlkNewStuffEngine *q;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QPointer<KNSCore::EngineBase> m_engine;
    #define KNS3 KNSCore
    bool m_providersLoaded{false};
    bool m_wantUpdates{false};
#elif QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    QPointer<KNSCore::Engine> m_engine;
    bool m_providersLoaded{false};
    bool m_wantUpdates{false};
    bool m_wantInstalled{false};
#else
    QPointer<KNS3::DownloadManager> m_engine;
#endif
    QEventLoop m_loop;
    AlkNewStuffEntryMap m_installedEntries;

    explicit Private(AlkNewStuffEngine *parent);
    ~Private();

    bool init(const QString &configFile);
    void checkForUpdates();
    void checkForInstalled();

    const AlkNewStuffEntryList installedEntries();

public Q_SLOTS:
    void slotUpdatesAvailable(const KNS3::Entry::List &entries);
    void slotEntriesAvailable(const KNS3::Entry::List &entries);
};

AlkNewStuffEngine::Private::Private(AlkNewStuffEngine *parent)
    : q(parent), m_engine(nullptr) {}

AlkNewStuffEngine::Private::~Private() { delete m_engine; }

bool AlkNewStuffEngine::Private::init(const QString &configFile)
{
    bool state = false;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    m_engine = new KNSCore::EngineBase(this);
    connect(m_engine, &KNSCore::EngineBase::signalProvidersLoaded, this, [this]()
    {
        alkDebug() << "providers loaded";
        m_providersLoaded = true;
        if (m_wantUpdates) {
            checkForUpdates();
        }
    });
#elif KNEWSTUFF_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    m_engine = new KNSCore::Engine(this);
    m_engine->setPageSize(100);

    connect(m_engine, &KNSCore::Engine::signalProvidersLoaded, this, [this]()
    {
        alkDebug() << "providers loaded";
        m_providersLoaded = true;
        if (m_wantInstalled)
            m_engine->checkForInstalled();
        else if (m_wantUpdates)
            m_engine->checkForUpdates();
    });

    connect(m_engine, &KNSCore::Engine::signalUpdateableEntriesLoaded, this, [this](const KNSCore::EntryInternal::List &entries)
    {
        alkDebug() << entries.size() << " updates loaded" ;
        AlkNewStuffEntryList updateEntries;
        for (const KNSCore::EntryInternal &entry : entries) {
            AlkNewStuffEntry e;
            e.category = entry.category();
            e.id = entry.uniqueId();
            e.installedFiles = entry.installedFiles();
            e.name = entry.name();
            e.providerId = entry.providerId();
            e.status =
                static_cast<AlkNewStuffEntry::Status>(entry.status());
            e.version = entry.version();
            updateEntries.append(e);
            if ((e.status == AlkNewStuffEntry::Installed || e.status == AlkNewStuffEntry::Updateable) && !m_installedEntries.contains(e.id))
                m_installedEntries[e.id] = e;
            alkDebug() << e.name << toString(e.status);
        }
        if (updateEntries.size() > 0) {
            alkDebug() << "emitting" << updateEntries.size() << "entries";
            Q_EMIT q->updatesAvailable(updateEntries);
        }
    });

    connect(m_engine, &KNSCore::Engine::signalEntriesLoaded, this, [this](const KNSCore::EntryInternal::List &entries)
    {
        alkDebug() << entries.size() << " entries loaded" ;
        for (const KNSCore::EntryInternal &entry : entries) {
            AlkNewStuffEntry e;
            e.category = entry.category();
            e.id = entry.uniqueId();
            e.installedFiles = entry.installedFiles();
            e.name = entry.name();
            e.providerId = entry.providerId();
            e.status =
                static_cast<AlkNewStuffEntry::Status>(entry.status());
            e.version = entry.version();
            if (!this->m_installedEntries.contains(e.id)) {
                this->m_installedEntries[e.id] = e;
                alkDebug() << e.name << toString(e.status) << e.installedFiles;
            }
        }
        if (this->m_installedEntries.size() > 0) {
            alkDebug() << "emitting" << this->m_installedEntries.size() << "entries";
            Q_EMIT q->entriesAvailable(this->m_installedEntries.values());
        }
    });
    state = m_engine->init(configFile);
    if (!state)
        return false;
#else
    m_engine = new KNS3::DownloadManager(configFile, this);
    // no chance get the state
    state = true;

    connect(m_engine, SIGNAL(searchResult(KNS3::Entry::List)), this,
            SLOT(slotUpdatesAvailable(KNS3::Entry::List)));
#endif
    return state;
}

void AlkNewStuffEngine::Private::checkForUpdates()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    alkDebug() << "FIXME Qt6: no checkforUpdates() - how to proceed ?";
#elif QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    if (m_providersLoaded && !m_wantUpdates) {
        m_engine->checkForUpdates();
    } else
        m_wantUpdates = true;
#else
    m_engine->checkForUpdates();
#endif
}

void AlkNewStuffEngine::Private::checkForInstalled()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    alkDebug() << "FIXME Qt6: no checkforUpdates() - how to proceed ?";
#elif QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    if (m_providersLoaded && !m_wantInstalled) {
        m_engine->checkForInstalled();
    } else
        m_wantInstalled = true;
#else
    m_engine->checkForInstalled();
#endif
}

const AlkNewStuffEntryList AlkNewStuffEngine::Private::installedEntries()
{
    if (m_installedEntries.empty()) {
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
        alkDebug() << "FIXME Qt6:";
#elif QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        m_engine->checkForInstalled();
#else
        m_engine->setSearchTerm("*");
        m_engine->requestData(0, 1000);
        QEventLoop loop;
        disconnect(m_engine, SIGNAL(searchResult(KNS3::Entry::List)), this,
                SLOT(slotUpdatesAvailable(KNS3::Entry::List)));
        connect(m_engine, SIGNAL(searchResult(KNS3::Entry::List)), this,
                SLOT(slotEntriesAvailable(KNS3::Entry::List)));
        m_engine->search(0, 1000);
#endif
        m_loop.exec();
    }

    AlkNewStuffEntryList result;
    for (const AlkNewStuffEntry &entry : m_installedEntries) {
        if (entry.status == AlkNewStuffEntry::Installed || entry.status == AlkNewStuffEntry::Updateable)
            result.append(entry);
    }
    return result;
}

void AlkNewStuffEngine::Private::slotUpdatesAvailable(const KNS3::Entry::List &entries)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    Q_UNUSED(entries);
#else
    alkDebug() << "updates loaded";
    AlkNewStuffEntryList updateEntries;
    for (const KNS3::Entry &entry : entries) {
        AlkNewStuffEntry e;
        e.category = entry.category();
        e.id = entry.id();
        e.installedFiles = entry.installedFiles();
        e.name = entry.name();
        e.providerId = entry.providerId();
        e.status = static_cast<AlkNewStuffEntry::Status>(entry.status());
        e.version = entry.version();
        updateEntries.append(e);

        alkDebug() << e.name << toString(e.status);
    }

    Q_EMIT q->updatesAvailable(updateEntries);
#endif
}

void AlkNewStuffEngine::Private::slotEntriesAvailable(const KNS3::Entry::List &entries)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    Q_UNUSED(entries);
#else
    alkDebug() << "entries loaded";
    for (const KNS3::Entry &entry : entries) {
        AlkNewStuffEntry e;
        e.category = entry.category();
        e.id = entry.id();
        e.installedFiles = entry.installedFiles();
        e.name = entry.name();
        e.providerId = entry.providerId();
        e.status = static_cast<AlkNewStuffEntry::Status>(entry.status());
        e.version = entry.version();
        m_availableEntries.append(e);

        alkDebug() << e.name << toString(e.status);
    }
    disconnect(m_engine, SIGNAL(searchResult(KNS3::Entry::List)), this,
            SLOT(slotEntriesAvailable(KNS3::Entry::List)));
    connect(m_engine, SIGNAL(searchResult(KNS3::Entry::List)), this,
            SLOT(slotUpdatesAvailable(KNS3::Entry::List)));
    m_loop.exit();
#endif
}

AlkNewStuffEngine::AlkNewStuffEngine(QObject *parent)
    : QObject{parent}
    , d(new Private(this))
{
}

bool AlkNewStuffEngine::init(const QString &configFile)
{
    bool result = d->init(configFile);
    return result;
}

void AlkNewStuffEngine::checkForUpdates()
{
    d->checkForUpdates();
}

void AlkNewStuffEngine::checkForInstalled()
{
    d->checkForInstalled();
}

AlkNewStuffEntryList AlkNewStuffEngine::installedEntries() const
{
    return d->installedEntries();
}

void AlkNewStuffEngine::reload()
{
    d->m_installedEntries.clear();
}

const char *toString(AlkNewStuffEntry::Status status)
{
    switch(status) {
        case AlkNewStuffEntry::Invalid: return "Invalid";
        case AlkNewStuffEntry::Downloadable: return "Downloadable";
        case AlkNewStuffEntry::Installed: return "Installed";
        case AlkNewStuffEntry::Updateable: return "Updateable";
        case AlkNewStuffEntry::Deleted: return "Deleted";
        case AlkNewStuffEntry::Installing: return "Installing";
        case AlkNewStuffEntry::Updating: return "Updating";
    }
    return "";
}

#include "alknewstuffengine.moc"
