#ifndef ALKONLINEQUOTESPROFILE_P_H
#define ALKONLINEQUOTESPROFILE_P_H

#include "alkonlinequotesprofile.h"
#include "alkfinancequoteprocess.h"

#include <QObject>
#include <QApplication>
#include <QDir>
#include <QString>
#include <QtDebug>
#include <QFileInfo>

#include <KConfig>
#include <KConfigGroup>
#include <KGlobal>
#include <KStandardDirs>
#include <knewstuff3/downloadmanager.h>

class AlkOnlineQuotesProfile::Private : public QObject
{
    Q_OBJECT
public:
    AlkOnlineQuotesProfile *m_p;
    QString m_name;
    QString m_GHNSFile;
    QString m_GHNSFilePath;
    QString m_kconfigFile;
    AlkOnlineQuotesProfileManager *m_profileManager;
    KNS3::DownloadManager *m_manager;
    KConfig *m_config;
    Type m_type;
    static QString m_financeQuoteScriptPath;
    static QStringList m_financeQuoteSources;

    Private(AlkOnlineQuotesProfile *p)
        : m_p(p)
        , m_profileManager(0)
        , m_manager(0)
        , m_config(0)
        , m_type(Type::Undefined)
    {

        if (m_financeQuoteScriptPath.isEmpty()) {
            m_financeQuoteScriptPath = KGlobal::dirs()->findResource("appdata",
                                                                     QString("misc/financequote.pl"));
        }
    }

    ~Private()
    {
        delete m_manager;
        delete m_config;
    }

    void checkUpdates()
    {
        m_manager = new KNS3::DownloadManager(m_p->hotNewStuffConfigFile(), this);
        // to know when checking for updates is done
        connect(m_manager, SIGNAL(searchResult(KNS3::Entry::List)), this,
                SLOT(slotUpdatesFound(KNS3::Entry::List)));
        // to know about finished installations
        connect(m_manager, SIGNAL(entryStatusChanged(KNS3::Entry)), this,
                SLOT(entryStatusChanged(KNS3::Entry)));
        // start checking for updates
        m_manager->checkForUpdates();
    }

public Q_SLOTS:
    void slotUpdatesFound(const KNS3::Entry::List &updates)
    {
        foreach (const KNS3::Entry &entry, updates) {
            qDebug() << entry.name();
            m_manager->installEntry(entry);
        }
    }

    // to know about finished installations
    void entryStatusChanged(const KNS3::Entry &entry)
    {
        qDebug() << __FUNCTION__ << entry.summary();
    }

    const QStringList quoteSourcesNative()
    {
        //KSharedConfigPtr kconfig = KGlobal::config();
        KConfig config(m_kconfigFile);
        KConfig *kconfig = &config;
        QStringList groups = kconfig->groupList();

        QStringList::Iterator it;
        QRegExp onlineQuoteSource(QString("^Online-Quote-Source-(.*)$"));

        // get rid of all 'non online quote source' entries
        for (it = groups.begin(); it != groups.end(); it = groups.erase(it)) {
            if (onlineQuoteSource.indexIn(*it) >= 0) {
                // Insert the name part
                it = groups.insert(it, onlineQuoteSource.cap(1));
                ++it;
            }
        }

        // Set up each of the default sources.  These are done piecemeal so that
        // when we add a new source, it's automatically picked up. And any changes
        // are also picked up.
        QMap<QString, AlkOnlineQuoteSource> defaults = defaultQuoteSources();
        QMap<QString, AlkOnlineQuoteSource>::iterator it_source = defaults.begin();
        while (it_source != defaults.end()) {
            if (!groups.contains((*it_source).name())) {
                groups += (*it_source).name();
                (*it_source).write();
                kconfig->sync();
            }
            ++it_source;
        }

        return groups;
    }

    const QStringList quoteSourcesFinanceQuote()
    {
        if (m_financeQuoteSources.empty()) { // run the process one time only
            // since this is a static function it can be called without constructing an object
            // so we need to make sure that m_financeQuoteScriptPath is properly initialized
            if (m_financeQuoteScriptPath.isEmpty()) {
                m_financeQuoteScriptPath = KGlobal::dirs()->findResource("appdata",
                                                                         QString("financequote.pl"));
            }
            AlkFinanceQuoteProcess getList;
            getList.launch(m_financeQuoteScriptPath);
            while (!getList.isFinished()) {
                qApp->processEvents();
            }
            m_financeQuoteSources = getList.getSourceList();
        }
        return m_financeQuoteSources;
    }

    const QStringList quoteSourcesSkrooge()
    {
        return quoteSourcesGHNS();
    }

    const QStringList quoteSourcesGHNS()
    {
        QStringList sources;
        QString relPath = m_GHNSFilePath;

        foreach (const QString &file,
                 KStandardDirs().findAllResources("data", relPath + QString::fromLatin1("/*.txt"))) {
            QFileInfo f(file);
            QString file2 = f.completeBaseName();
            AlkOnlineQuoteSource source(file2, m_p);
            if (source.isEmpty()) {
                qDebug() << "skipping" << file2;
                continue;
            }
            if (!sources.contains(file2)) {
                sources.push_back(file2);
            }
        }

        return sources;
    }

    const AlkOnlineQuotesProfile::Map defaultQuoteSources()
    {
        QMap<QString, AlkOnlineQuoteSource> result;

        // Use fx-rate.net as the standard currency exchange rate source until
        // we have the capability to use more than one source. Use a neutral
        // name for the source.

        switch (m_p->type()) {
        case AlkOnlineQuotesProfile::Type::None:
        case AlkOnlineQuotesProfile::Type::Alkimia4:
        case AlkOnlineQuotesProfile::Type::Alkimia5: {
            AlkOnlineQuoteSource source("Alkimia Currency",
                                        "https://fx-rate.net/%1/%2",
                                        QString(), // symbolregexp
                                        "1[ a-zA-Z]+=</span><br */?> *(\\d+\\.\\d+)",
                                        "updated\\s\\d+:\\d+:\\d+\\(\\w+\\)\\s+(\\d{1,2}/\\d{2}/\\d{4})",
                                        "%d/%m/%y",
                                        true // skip HTML stripping
                                        );
            source.setProfile(m_p);
            result[source.name()] = source;
            source.setName(source.name() + ".webkit");
            result[source.name()] = source;
            break;
        }
        default:
            break;
        }
        return result;
    }
};


#endif // ALKONLINEQUOTESPROFILE_P_H
