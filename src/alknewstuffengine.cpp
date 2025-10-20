/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alknewstuffengine.h"

#include "alkdebug.h"
#include "alknewstuffenginebase.h"
#include "alknewstuffentry_p.h"

#include <QEventLoop>
#include <QPointer>
#include <QWidget>

class AlkNewStuffEngine::Private : public QObject
{
    Q_OBJECT
public:
    AlkNewStuffEngine *q;
    QPointer<AlkNewStuffEngineBase> m_engine;
    QSharedPointer<AlkNewStuffCache> m_cache;
    bool m_providersLoaded{false};
    bool m_wantUpdates{false};
    QEventLoop m_loop;

    explicit Private(AlkNewStuffEngine *parent);
    ~Private();

    bool init(const QString &configFile);
    void checkForUpdates();

    const AlkNewStuffEntryList installedEntries();

public Q_SLOTS:
    void slotUpdatesAvailable(const EntryList &entries);
};

AlkNewStuffEngine::Private::Private(AlkNewStuffEngine *parent)
    : q(parent)
    , m_engine(nullptr)
    , m_cache(nullptr)
{
}

AlkNewStuffEngine::Private::~Private()
{
    delete m_engine;
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    delete m_cache;
#endif
}

bool AlkNewStuffEngine::Private::init(const QString &configFile)
{
    bool state = false;
#if KNEWSTUFF_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    m_engine = new AlkNewStuffEngineBase(this);
    state = m_engine->init(configFile);
    if (!state)
        return false;
    m_cache = m_engine->cache();

    q->connect(m_engine, &AlkNewStuffEngineBase::signalErrorCode, q, [](const ErrorCode &, const QString &message, const QVariant &) {
        alkDebug() << message;
    });

    connect(m_engine, &AlkNewStuffEngineBase::signalProvidersLoaded, this, [this]()
    {
        alkDebug() << "providers loaded";
        m_providersLoaded = true;
        m_engine->reloadEntries();
        alkDebug() << "cache" << m_engine->cache() << m_engine->cache()->registry();
        if (m_wantUpdates)
            m_engine->checkForUpdates();
    });

    connect(m_engine, &AlkNewStuffEngineBase::signalUpdateableEntriesLoaded, this, [this](const EntryList &entries)
    {
        alkDebug() << entries.size() << "updates loaded";
        AlkNewStuffEntryList updateEntries;
        toAlkEntryList(updateEntries, entries);
        alkDebug() << updateEntries;
        Q_EMIT q->updatesAvailable(updateEntries);
    });
#else
    m_engine = new KNS3::DownloadManager(configFile, this);
    QFileInfo f(configFile);
    m_cache = new Cache(f.baseName());
    m_cache->readRegistry();
    // no chance get the state
    state = true;

    connect(m_engine, SIGNAL(searchResult(KNS3::Entry::List)), this,
            SLOT(slotUpdatesAvailable(KNS3::Entry::List)));
#endif
    return state;
}

void AlkNewStuffEngine::Private::checkForUpdates()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    if (m_providersLoaded && !m_wantUpdates) {
        m_engine->checkForUpdates();
    } else
        m_wantUpdates = true;
#else
    m_engine->checkForUpdates();
#endif
}

const AlkNewStuffEntryList AlkNewStuffEngine::Private::installedEntries()
{
    AlkNewStuffEntryList result;
    if (m_cache)
        toAlkEntryList(result, m_cache->registry());

    alkDebug() << result;
    return result;
}

void AlkNewStuffEngine::Private::slotUpdatesAvailable(const EntryList &entries)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    Q_UNUSED(entries);
#else
    alkDebug() << entries.size() << "updates loaded";
    AlkNewStuffEntryList updateEntries;
    toAlkEntryList(updateEntries, entries);
    alkDebug() << entries;

    Q_EMIT q->updatesAvailable(updateEntries);
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

AlkNewStuffEntryList AlkNewStuffEngine::installedEntries() const
{
    return d->installedEntries();
}

void AlkNewStuffEngine::reload()
{
    d->m_cache->readRegistry();
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
