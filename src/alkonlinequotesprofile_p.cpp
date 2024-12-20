/*
    SPDX-FileCopyrightText: 2019 Thomas Baumgart tbaumgart @kde.org
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alkonlinequotesprofile_p.h"

#include "alkdebug.h"
#include "alkutils.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QRegularExpression>
#else
#include <KConfig>
#include <KDebug>
#endif

// define static members
QString AlkOnlineQuotesProfile::Private::m_financeQuoteScriptPath;
QStringList AlkOnlineQuotesProfile::Private::m_financeQuoteSources;

bool AlkOnlineQuotesProfile::Private::setupFinanceQuoteScriptPath()
{
    if (m_financeQuoteScriptPath.isEmpty()) {
        m_financeQuoteScriptPath = AlkUtils::locateDataFile("misc/financequote.pl");
    }
    return !m_financeQuoteScriptPath.isEmpty();
}

AlkOnlineQuotesProfile::Private::Private(AlkOnlineQuotesProfile *p)
    : m_p(p)
    , m_profileManager(0)
    , m_engine(new AlkNewStuffEngine)
    , m_config(0)
    , m_type(Type::Undefined)
{
#ifdef ENABLE_FINANCEQUOTE
    setupFinanceQuoteScriptPath();
#endif
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
    connect(m_engine, SIGNAL(updatesAvailable(AlkNewStuffEntryList)), this,
            SLOT(slotUpdatesAvailable(AlkNewStuffEntryList)));
#else
    connect(m_engine, &AlkNewStuffEngine::updatesAvailable, this,
            &AlkOnlineQuotesProfile::Private::slotUpdatesAvailable);
#endif
}

AlkOnlineQuotesProfile::Private::~Private()
{
    delete m_engine;
}

QString AlkOnlineQuotesProfile::Private::GHNSId(const QString &name)
{
    for (const AlkNewStuffEntry &entry : installedNewStuffEntries()) {
        if (entry.name == name)
            return entry.id;
    }
    return QString();
}

QString AlkOnlineQuotesProfile::Private::GHNSFilePath(const QString &name)
{
    // TODO: find a more efficient way to get the installed file
    for (const AlkNewStuffEntry &entry : installedNewStuffEntries()) {
        if (entry.name == name)
            return entry.installedFiles.size() > 0 ? entry.installedFiles[0] : QString();
    }
    return QString();
}

QString AlkOnlineQuotesProfile::Private::GHNSName(const QString &id)
{
    for (const AlkNewStuffEntry &entry : installedNewStuffEntries()) {
        if (entry.id == id)
            return entry.name;
    }
    return QString();
}

const QStringList AlkOnlineQuotesProfile::Private::quoteSourcesNative()
{
    auto kconfig = KSharedConfig::openConfig(m_kconfigFile, KConfig::SimpleConfig);
    QStringList groups = kconfig->groupList();

    QStringList::Iterator it;
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
    QRegExp onlineQuoteSource(QString("^Online-Quote-Source-(.*)$"));

    // get rid of all 'non online quote source' entries
    for (it = groups.begin(); it != groups.end(); it = groups.erase(it)) {
        if (onlineQuoteSource.indexIn(*it) >= 0) {
            // Insert the name part
            it = groups.insert(it, onlineQuoteSource.cap(1));
            ++it;
        }
    }
#else
    QRegularExpression onlineQuoteSource(QLatin1String("^Online-Quote-Source-(.*)$"));

    // get rid of all 'non online quote source' entries
    for (it = groups.begin(); it != groups.end(); it = groups.erase(it)) {
        const auto match = onlineQuoteSource.match(*it);
        if (match.hasMatch()) {
            // Insert the name part
            it = groups.insert(it, match.captured(1));
            ++it;
        }
    }
#endif
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

#ifdef ENABLE_FINANCEQUOTE
const QStringList AlkOnlineQuotesProfile::Private::quoteSourcesFinanceQuote()
{
    if (m_financeQuoteSources.empty()) { // run the process one time only
        // since this is a static function it can be called without constructing an object
        // so we need to make sure that m_financeQuoteScriptPath is properly initialized
        if (setupFinanceQuoteScriptPath()) {
            AlkFinanceQuoteProcess testList;
            testList.testLaunch(m_financeQuoteScriptPath);
            while (!testList.isFinished()) {
                qApp->processEvents();
            }

            if (testList.exitCode() == 0) {
                AlkFinanceQuoteProcess getList;
                getList.launch(m_financeQuoteScriptPath);
                while (!getList.isFinished()) {
                    qApp->processEvents();
                }
                m_financeQuoteSources = getList.getSourceList();
            }
        }
    }
    return m_financeQuoteSources;
}
#endif

const QStringList AlkOnlineQuotesProfile::Private::quoteSourcesSkrooge()
{
    return quoteSourcesGHNS();
}

const QStringList AlkOnlineQuotesProfile::Private::quoteSourcesGHNS()
{
    QStringList sources;
    QStringList files = AlkUtils::getDataFiles(m_GHNSFilePath, QStringList() << QStringLiteral("*.txt"));

    // add installed remote sources
    for (const AlkNewStuffEntry &entry : installedNewStuffEntries()) {
        AlkOnlineQuoteSource source(entry.name, m_p);
        if (entry.installedFiles.size() > 0)
            files.removeAll(entry.installedFiles[0]);
        if (source.isEmpty()) {
            alkDebug() << "skipping" << entry.name;
            continue;
        }
        if (!sources.contains(entry.id)) {
            alkDebug() << "adding published quote source" << entry.name;
            sources.push_back(entry.name);
        }
    }

    // add unpublished remote sources
    for (const QString &file : files) {
        QFileInfo f(file);
        QString name = f.completeBaseName();
        AlkOnlineQuoteSource source(name, m_p);
        if (source.isEmpty()) {
            alkDebug() << "skipping" << name;
            continue;
        }
        if (!sources.contains(name)) {
            alkDebug() << "adding unpublished remote quote source" << name;
            sources.push_back(name);
        }
    }
    return sources;
}

AlkOnlineQuotesProfile::Map AlkOnlineQuotesProfile::Private::quoteSourcesTesting()
{
    QMap<QString, AlkOnlineQuoteSource> result;

    AlkOnlineQuoteSource source1(AlkOnlineQuoteSource::testQuoteSource(
                                     "Alkimia:Test:1:Default:HTML"));
    source1.setProfile(m_p);
    source1.setDownloadType(AlkOnlineQuoteSource::Default);
    result[source1.name()] = source1;

    AlkOnlineQuoteSource source2(AlkOnlineQuoteSource::testQuoteSource(
                                     "Alkimia:Test:1:Javascript:HTML", false,
                                     AlkOnlineQuoteSource::Javascript, AlkOnlineQuoteSource::HTML));
    source2.setProfile(m_p);
    source2.setDownloadType(AlkOnlineQuoteSource::Javascript);
    result[source2.name()] = source2;

    AlkOnlineQuoteSource source4(AlkOnlineQuoteSource::testQuoteSource(
                                     "Alkimia:Test:1:Default:JSON", false,
                                     AlkOnlineQuoteSource::Default, AlkOnlineQuoteSource::JSON));
    source4.setProfile(m_p);
    source4.setDownloadType(AlkOnlineQuoteSource::Default);
    result[source4.name()] = source4;

    AlkOnlineQuoteSource source5(AlkOnlineQuoteSource::testQuoteSource(
                                     "Alkimia:Test:2:Default:JSON", true,
                                     AlkOnlineQuoteSource::Default, AlkOnlineQuoteSource::JSON));
    source5.setProfile(m_p);
    source5.setDownloadType(AlkOnlineQuoteSource::Default);
    result[source5.name()] = source5;

    return result;
}

const AlkOnlineQuotesProfile::Map AlkOnlineQuotesProfile::Private::defaultQuoteSources()
{
    QMap<QString, AlkOnlineQuoteSource> result;

    // Use fx-rate.net as the standard currency exchange rate source until
    // we have the capability to use more than one source. Use a neutral
    // name for the source.

    switch (m_p->type()) {
    case AlkOnlineQuotesProfile::Type::None:
    case AlkOnlineQuotesProfile::Type::Alkimia4:
    case AlkOnlineQuotesProfile::Type::Alkimia5:
    {
        AlkOnlineQuoteSource source(AlkOnlineQuoteSource::defaultCurrencyQuoteSource("Alkimia Currency"));
        source.setProfile(m_p);
        result[source.name()] = source;
#if defined(BUILD_WITH_WEBKIT) || defined(BUILD_WITH_WEBENGINE)
        AlkOnlineQuoteSource source2(AlkOnlineQuoteSource::defaultCurrencyQuoteSource("Alkimia Currency (Javascript)"));
        source2.setProfile(m_p);
        source2.setDownloadType(AlkOnlineQuoteSource::Javascript);
        result[source2.name()] = source2;
#endif
        break;
    }
    case AlkOnlineQuotesProfile::Type::Test:
        result = quoteSourcesTesting();
        break;
    default:
        break;
    }
    return result;
}

void AlkOnlineQuotesProfile::Private::updateQuoteSources(QStringList &sources)
{
    for (auto &s : QStringList(sources)) {
        if (s.endsWith(QLatin1String(".webkit"))) {
            AlkOnlineQuoteSource source(s, m_p);
            source.setName(source.name().replace(QLatin1String(".webkit"), QLatin1String(" (Javascript)")));
            source.setDownloadType(AlkOnlineQuoteSource::Javascript);
            source.write();
            sources.removeAll(s);
            sources.append(source.name());
        } else if (s.endsWith(QLatin1String(".css"))) {
            AlkOnlineQuoteSource source(s, m_p);
            source.setName(source.name().replace(QLatin1String(".css"), QLatin1String(" (Javascript/CSS)")));
            source.setDownloadType(AlkOnlineQuoteSource::Javascript);
            source.setDataFormat(AlkOnlineQuoteSource::CSS);
            source.write();
            sources.removeAll(s);
            sources.append(source.name());
            // TODO
        }
    }
}

QString AlkOnlineQuotesProfile::Private::dataRootPath()
{
    return QLibraryInfo::location(QLibraryInfo::PrefixPath) + "/share";
}

QString AlkOnlineQuotesProfile::Private::homeRootPath()
{
    if (m_type == Type::KMyMoney5 || m_type == Type::Alkimia5 || m_type == Type::Skrooge5 || m_type == Type::Test)
        return QDir::homePath();
    else if (m_type == Type::KMyMoney4 || m_type == Type::Alkimia4 || m_type == Type::Skrooge4) {
#ifdef Q_OS_WIN
        return qgetenv("APPDATA");
#else
        return QDir::homePath();
#endif
    } else {
        return QString();
    }
}

QString AlkOnlineQuotesProfile::Private::configPath()
{
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    if (m_type == Type::KMyMoney5)
        return QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);
    else if(m_type == Type::Alkimia5 || m_type == Type::Skrooge5 || m_type == Type::Test)
        return QString("%1/.config").arg(homeRootPath());
    else
#endif
        if (m_type == Type::KMyMoney4 || m_type == Type::Alkimia4 || m_type == Type::Skrooge4)
            return QString("%1/.kde4/share/config").arg(homeRootPath());
    return
            QString();
}

QString AlkOnlineQuotesProfile::Private::dataReadPath()
{
    if (m_type == Type::KMyMoney5 || m_type == Type::Alkimia5 || m_type == Type::Skrooge5 || m_type == Type::Test)
        return dataRootPath();
    else if (m_type == Type::KMyMoney4 || m_type == Type::Alkimia4 || m_type == Type::Skrooge4)
        return QString("%1/kde4/apps").arg(dataRootPath());
    return
            QString();
}

QString AlkOnlineQuotesProfile::Private::dataWritePath()
{
    if (m_type == Type::KMyMoney5 || m_type == Type::Alkimia5 || m_type == Type::Skrooge5 || m_type == Type::Test)
        return QString("%1/.local/share").arg(homeRootPath());
    else if (m_type == Type::KMyMoney4 || m_type == Type::Alkimia4 || m_type == Type::Skrooge4)
        return QString("%1/.kde4/share/apps").arg(homeRootPath());
    return
            QString();
}

void AlkOnlineQuotesProfile::Private::reload()
{
    m_installedNewStuffEntries.clear();
    m_engine->reload();
}

const AlkNewStuffEntryList &AlkOnlineQuotesProfile::Private::installedNewStuffEntries()
{
    if (m_installedNewStuffEntries.isEmpty())
        m_installedNewStuffEntries = m_engine->installedEntries();
    return m_installedNewStuffEntries;
}

void AlkOnlineQuotesProfile::Private::slotUpdatesAvailable(const AlkNewStuffEntryList &updates)
{
    for (auto &entry : updates) {
        alkDebug() << "update available in profile" << m_p->name() << "for"
                 << entry.name << entry.version << entry.id << entry.category
                 << entry.providerId;
        Q_EMIT m_p->updateAvailable(m_p->name(), entry.name);
    }
}
