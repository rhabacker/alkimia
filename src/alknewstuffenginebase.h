/*
    SPDX-FileCopyrightText: 2025 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#pragma once

#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <KNSCore/Cache>
#include <KNSCore/EngineBase>
#include <KNSCore/Provider>
#include <KNSCore/ResultsStream>
#define KNS3 KNSCore
using EntryList = KNSCore::Entry::List;
using ErrorCode = KNSCore::ErrorCode::ErrorCode;

class AlkNewStuffEngineBase : public KNSCore::EngineBase
{
    Q_OBJECT
public:
    AlkNewStuffEngineBase(QObject *parent = nullptr)
        : KNSCore::EngineBase(parent)
    {
    }

    void checkForUpdates()
    {
        for (const QSharedPointer<KNSCore::Provider> &p : providers()) {
            connect(p.get(), &KNSCore::Provider::loadingFinished, this, [this](const KNSCore::Provider::SearchRequest &, const KNSCore::Entry::List &entries)
            {
                Q_EMIT signalUpdateableEntriesLoaded(entries);
            });
            KNSCore::Provider::SearchRequest request(KNSCore::Provider::Newest, KNSCore::Provider::Updates);
            p->loadEntries(request);
        }
    }

    void reloadEntries()
    {
        for (const QSharedPointer<KNSCore::Provider> &p : providers()) {
            connect(p.get(), &KNSCore::Provider::loadingFinished, this, [this](const KNSCore::Provider::SearchRequest &, const EntryList &entries)
            {
                Q_EMIT signalEntriesLoaded(entries);
            });
            KNSCore::Provider::SearchRequest request(KNSCore::Provider::Newest, KNSCore::Provider::Updates);
            p->loadEntries(request);
        }
    }

Q_SIGNALS:
    void signalUpdateableEntriesLoaded(const EntryList &entries);
    void signalEntriesLoaded(const EntryList &entries);
};

typedef KNSCore::Cache AlkNewStuffCache;

#elif QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <KNSCore/Cache>
#include <knewstuff_version.h>
#include <knscore/engine.h>
typedef KNSCore::Engine AlkNewStuffEngineBase;
typedef KNSCore::Cache AlkNewStuffCache;
using EntryList = KNSCore::EntryInternal::List;
using ErrorCode = KNSCore::ErrorCode;
#else
#include <knewstuff3/core/cache.h>
#include <knewstuff3/downloadmanager.h>
#define KNEWSTUFF_VERSION 0
typedef KNS3::DownloadManager AlkNewStuffEngineBase;
typedef Cache AlkNewStuffCache;
using EntryList = KNS3::Entry::List;
#endif
