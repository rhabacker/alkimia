/*
    SPDX-FileCopyrightText: 2004 Ace Jones acejones @users.sourceforge.net
    SPDX-FileCopyrightText: 2019 Thomas Baumgart tbaumgart @kde.org
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "alkonlinequote_p.h"

#include "alkdateformat.h"
#include "alkexception.h"
#ifdef ENABLE_FINANCEQUOTE
#include "alkfinancequoteprocess.h"
#include "alkonlinequoteprocess.h"
#endif
#include "alkonlinequotesprofile.h"
#include "alkonlinequotesprofilemanager.h"
#include "alkonlinequotesource.h"
#include "alkimia/alkversion.h"
#include "alkwebpage.h"


#ifdef BUILD_WITH_QTNETWORK
    #include <QNetworkAccessManager>
    #include <QNetworkRequest>
    #include <QNetworkReply>
    #include <QNetworkProxyFactory>
#else
    #include <KIO/Scheduler>
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    #include <KLocalizedString>
#ifndef BUILD_WITH_QTNETWORK
    #include <KIO/Job>
#endif
    #include <QDebug>
    #include <QTemporaryFile>
    #define kDebug(a) qDebug()
    #define KIcon QIcon
#else
    #include <KDebug>
    #include <KGlobal>
    #include <KLocale>
#ifndef BUILD_WITH_QTNETWORK
    #include <kio/netaccess.h>
#endif
#endif

#include <KConfigGroup>
#include <KEncodingProber>
#include <KProcess>
#include <KShell>

#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
    #include <QRegExp>
    using Regex = QRegExp;
    #define hasRegexMatch(a) indexIn(a) != -1
    #define capturedText(txt, index) cap(index)
#else
    #include <QRegularExpression>
    using Regex = QRegularExpression;
    #define hasRegexMatch(a) match(a).hasMatch()
    #define capturedText(txt, index) match(txt).captured(index)
#endif

#ifndef I18N_NOOP
#include <KLazyLocalizedString>
#endif

AlkOnlineQuote::Private::Private(AlkOnlineQuote *parent)
    : m_p(parent)
    , m_eventLoop(nullptr)
    , m_ownProfile(false)
    , m_timeout(-1)
{
#ifdef ENABLE_FINANCEQUOTE
    connect(&m_filter, SIGNAL(processExited(QString)), this, SLOT(slotParseQuote(QString)));
#endif
}

AlkOnlineQuote::Private::~Private()
{
    if (m_ownProfile)
        delete m_profile;
}

#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
int AlkOnlineQuote::Private::dbgArea()
{
    static int s_area = KDebug::registerArea("Alkimia (AlkOnlineQuote)");
    return s_area;
}
#endif

bool AlkOnlineQuote::Private::initLaunch(const QString &_symbol, const QString &_id, const QString &_source)
{
    m_symbol = _symbol;
    m_id = _id;
    m_errors = Errors::None;

    Q_EMIT m_p->status(QString("(Debug) symbol=%1 id=%2...").arg(_symbol, _id));

    // Get sources from the config file
    QString source = _source;
    if (source.isEmpty()) {
        source = "KMyMoney Currency";
    }

    if (!m_profile->quoteSources().contains(source)) {
        Q_EMIT m_p->error(i18n("Source <placeholder>%1</placeholder> does not exist.", source));
        m_errors |= Errors::Source;
        return false;
    }

    //m_profile->createSource(source);
    m_source = AlkOnlineQuoteSource(source, m_profile);

    KUrl url;

    // if the source has room for TWO symbols..
    if (m_source.url().contains("%2")) {
        // this is a two-symbol quote.  split the symbol into two.  valid symbol
        // characters are: 0-9, A-Z and the dot.  anything else is a separator
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
        QRegExp splitrx("([0-9a-z\\.]+)[^a-z0-9]+([0-9a-z\\.]+)", Qt::CaseInsensitive);
        // if we've truly found 2 symbols delimited this way...
        if (splitrx.indexIn(m_symbol) != -1) {
            url = KUrl(m_source.url().arg(splitrx.cap(1), splitrx.cap(2)));
        } else {
            kDebug(Private::dbgArea()) << QString("AlkOnlineQuote::Private::initLaunch() did not find 2 symbols in '%1'").arg(m_symbol);
        }
#else
        QRegularExpression splitrx("([0-9a-z\\.]+)[^a-z0-9]+([0-9a-z\\.]+)", QRegularExpression::CaseInsensitiveOption);
        const auto match = splitrx.match(m_symbol);
        // if we've truly found 2 symbols delimited this way...
        if (match.hasMatch()) {
            url = KUrl(m_source.url().arg(match.captured(1), match.captured(2)));
        } else {
            kDebug(Private::dbgArea()) << QString("AlkOnlineQuote::Private::initLaunch() did not find 2 symbols in '%1'").arg(m_symbol);
        }
#endif
    } else {
        // a regular one-symbol quote
        url = KUrl(m_source.url().arg(m_symbol));
    }

    m_url = url;

    return true;
}

void AlkOnlineQuote::Private::slotLoadFinishedHtmlParser(bool ok)
{
    if (!ok) {
        Q_EMIT m_p->error(i18n("Unable to fetch url for %1", m_symbol));
        m_errors |= Errors::URL;
        Q_EMIT m_p->failed(m_id, m_symbol);
    } else {
        // parse symbol
        slotParseQuote(AlkOnlineQuotesProfileManager::instance().webPage()->toHtml());
    }
    if (m_eventLoop)
        m_eventLoop->exit();
}

void AlkOnlineQuote::Private::slotLoadFinishedCssSelector(bool ok)
{
    if (!ok) {
        Q_EMIT m_p->error(i18n("Unable to fetch url for %1", m_symbol));
        m_errors |= Errors::URL;
        Q_EMIT m_p->failed(m_id, m_symbol);
    } else {
        AlkWebPage *webPage = AlkOnlineQuotesProfileManager::instance().webPage();
        // parse symbol
        QString identifier = webPage->getFirstElement(m_source.idRegex());
        if (!identifier.isEmpty()) {
            Q_EMIT m_p->status(i18n("Symbol found: '%1'", identifier));
        } else {
            m_errors |= Errors::Symbol;
            Q_EMIT m_p->error(i18n("Unable to parse symbol for %1", m_symbol));
        }

        // parse price
        QString price = webPage->getFirstElement(m_source.priceRegex());
        bool gotprice = parsePrice(price);

        // parse date
        QString date = webPage->getFirstElement(m_source.dateRegex());
        bool gotdate = parseDate(date);

        if (gotprice && gotdate) {
            Q_EMIT m_p->quote(m_id, m_symbol, m_date, m_price);
        } else {
            Q_EMIT m_p->failed(m_id, m_symbol);
        }
    }
    if (m_eventLoop)
        m_eventLoop->exit();
}

void AlkOnlineQuote::Private::slotLoadStarted()
{
    Q_EMIT m_p->status(i18n("Fetching URL %1...", m_url.prettyUrl()));
}

bool AlkOnlineQuote::Private::launchWebKitCssSelector(const QString &_symbol, const QString &_id,
                                             const QString &_source)
{
    if (!initLaunch(_symbol, _id, _source)) {
        return false;
    }
    AlkWebPage *webPage = AlkOnlineQuotesProfileManager::instance().webPage();
    connect(webPage, SIGNAL(loadStarted()), this, SLOT(slotLoadStarted()));
    connect(webPage, SIGNAL(loadFinished(bool)), this,
            SLOT(slotLoadFinishedCssSelector(bool)));
    if (m_timeout != -1)
        QTimer::singleShot(m_timeout, this, SLOT(slotLoadTimeout()));
    webPage->setUrl(m_url);
    m_eventLoop = new QEventLoop;
    m_eventLoop->exec();
    delete m_eventLoop;
    m_eventLoop = nullptr;
    disconnect(webPage, SIGNAL(loadStarted()), this, SLOT(slotLoadStarted()));
    disconnect(webPage, SIGNAL(loadFinished(bool)), this,
               SLOT(slotLoadFinishedCssSelector(bool)));

    return !(m_errors & Errors::URL || m_errors & Errors::Price
             || m_errors & Errors::Date || m_errors & Errors::Data);
}

bool AlkOnlineQuote::Private::launchWebKitHtmlParser(const QString &_symbol, const QString &_id,
                                            const QString &_source)
{
    if (!initLaunch(_symbol, _id, _source)) {
        return false;
    }
    AlkWebPage *webPage = AlkOnlineQuotesProfileManager::instance().webPage();
    connect(webPage, SIGNAL(loadStarted()), this, SLOT(slotLoadStarted()));
    connect(webPage, SIGNAL(loadFinished(bool)), this, SLOT(slotLoadFinishedHtmlParser(bool)));
    if (m_timeout != -1)
        QTimer::singleShot(m_timeout, this, SLOT(slotLoadTimeout()));
    webPage->load(m_url, m_acceptLanguage);
    m_eventLoop = new QEventLoop;
    m_eventLoop->exec();
    delete m_eventLoop;
    m_eventLoop = nullptr;
    disconnect(webPage, SIGNAL(loadStarted()), this, SLOT(slotLoadStarted()));
    disconnect(webPage, SIGNAL(loadFinished(bool)), this, SLOT(slotLoadFinishedHtmlParser(bool)));

    return !(m_errors & Errors::URL || m_errors & Errors::Price
             || m_errors & Errors::Date || m_errors & Errors::Data);
}

bool AlkOnlineQuote::Private::launchNative(const QString &_symbol, const QString &_id,
                               const QString &_source)
{
    bool result = true;
    if (!initLaunch(_symbol, _id, _source)) {
        return false;
    }

    KUrl url = m_url;
    if (url.isLocalFile()) {
#ifdef ENABLE_FINANCEQUOTE
        result = processLocalScript(url);
#endif
    } else {
        slotLoadStarted();
        result = downloadUrl(url);
    }
    return result;
}

#ifdef ENABLE_FINANCEQUOTE
bool AlkOnlineQuote::Private::processLocalScript(const KUrl& url)
{
    Q_EMIT m_p->status(i18nc("The process x is executing", "Executing %1...", url.toLocalFile()));

    bool result = true;

    m_filter.clearProgram();
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
    m_filter << url.toLocalFile().split(' ', QString::SkipEmptyParts);
#else
    m_filter << url.toLocalFile().split(' ', Qt::SkipEmptyParts);
#endif
    m_filter.setSymbol(m_symbol);

    m_filter.setOutputChannelMode(KProcess::MergedChannels);
    m_filter.start();

    // This seems to work best if we just block until done.
    if (!m_filter.waitForFinished()) {
        Q_EMIT m_p->error(i18n("Unable to launch: %1", url.toLocalFile()));
        m_errors |= Errors::Script;
        result = slotParseQuote(QString());
    }
    return result;
}
#endif

bool AlkOnlineQuote::Private::processDownloadedFile(const KUrl& url, const QString& tmpFile)
{
  bool result = false;

  QFile f(tmpFile);
  if (f.open(QIODevice::ReadOnly)) {
    // Find out the page encoding and convert it to unicode
    QByteArray page = f.readAll();
    result = processDownloadedPage(url, page);
    f.close();
  } else {
    Q_EMIT m_p->error(i18n("Failed to open downloaded file"));
    m_errors |= Errors::URL;
    result = slotParseQuote(QString());
  }
  return result;
}

bool AlkOnlineQuote::Private::processDownloadedPage(const KUrl& url, const QByteArray& page)
{
    bool result = false;
    KEncodingProber prober(KEncodingProber::Universal);
    prober.feed(page);
    QTextCodec *codec = QTextCodec::codecForName(prober.encoding());
    if (!codec) {
      codec = QTextCodec::codecForLocale();
    }
    QString quote = codec->toUnicode(page);
    Q_EMIT m_p->status(i18n("URL found: %1...", url.prettyUrl()));
    if (AlkOnlineQuotesProfileManager::instance().webPageEnabled())
      AlkOnlineQuotesProfileManager::instance().webPage()->setContent(quote.toLocal8Bit());
    result = slotParseQuote(quote);
    return result;
}

#ifndef BUILD_WITH_QTNETWORK
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)

bool AlkOnlineQuote::Private::downloadUrl(const QUrl& url)
{
    // Create a temporary filename (w/o leaving the file on the filesystem)
    // In case the file is still present, the KIO::file_copy operation cannot
    // be performed on some operating systems (Windows).
    auto tmpFile = new QTemporaryFile;
    tmpFile->open();
    auto tmpFileName = QUrl::fromLocalFile(tmpFile->fileName());
    delete tmpFile;

    m_eventLoop = new QEventLoop;
    KJob *job = KIO::file_copy(url, tmpFileName, -1, KIO::HideProgressInfo);
    connect(job, SIGNAL(result(KJob*)), this, SLOT(downloadUrlDone(KJob*)));

    if (m_timeout != -1)
        QTimer::singleShot(m_timeout, this, SLOT(slotLoadTimeout()));
    auto result = m_eventLoop->exec(QEventLoop::ExcludeUserInputEvents);
    delete m_eventLoop;
    m_eventLoop = nullptr;

    return result;
}

void AlkOnlineQuote::Private::downloadUrlDone(KJob* job)
{
  QString tmpFileName = dynamic_cast<KIO::FileCopyJob*>(job)->destUrl().toLocalFile();
  QUrl url = dynamic_cast<KIO::FileCopyJob*>(job)->srcUrl();

  bool result;
  if (!job->error()) {
    qDebug() << "Downloaded" << tmpFileName << "from" << url;
    result = processDownloadedFile(url, tmpFileName);
  } else {
    Q_EMIT m_p->error(job->errorString());
    m_errors |= Errors::URL;
    result = slotParseQuote(QString());
  }
  m_eventLoop->exit(result);
}

#else // QT_VERSION

// This is simply a placeholder. It is unused but needs to be present
// to make the linker happy (since the declaration of the slot cannot
// be made dependendant on QT_VERSION with the Qt4 moc compiler.
void AlkOnlineQuote::Private::downloadUrlDone(KJob* job)
{
    Q_UNUSED(job);
}

bool AlkOnlineQuote::Private::downloadUrl(const KUrl& url)
{
    bool result = false;

    QString tmpFile;
    if (KIO::NetAccess::download(url, tmpFile, nullptr)) {
        // kDebug(Private::dbgArea()) << "Downloaded " << tmpFile;
        kDebug(Private::dbgArea()) << "Downloaded" << tmpFile << "from" << url;
        result = processDownloadedFile(url, tmpFile);
        KIO::NetAccess::removeTempFile(tmpFile);
    } else {
        Q_EMIT m_p->error(KIO::NetAccess::lastErrorString());
        m_errors |= Errors::URL;
        result = slotParseQuote(QString());
    }
    return result;
}

#endif // QT_VERSION
#else // BUILD_WITH_QTNETWORK

void AlkOnlineQuote::Private::downloadUrlDone(QNetworkReply *reply)
{
    int result = 0;
    if (reply->error() == QNetworkReply::NoError) {
        QUrl newUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
        if (!newUrl.isEmpty() && newUrl != reply->url()) {
            m_url = reply->url().resolved(newUrl);
            // TODO migrate to i18n()
            Q_EMIT m_p->status(QString("<font color=\"orange\">%1</font>")
            #ifdef I18N_NOOP
                            .arg(I18N_NOOP("The URL has been redirected; check an update of the online quote URL")));
            #else
                            .arg(kli18n("The URL has been redirected; check an update of the online quote URL").untranslatedText()));
            #endif
            result = 2;
        } else {
            kDebug(Private::dbgArea()) << "Downloaded data from" << reply->url();
            result = processDownloadedPage(KUrl(reply->url()), reply->readAll()) ? 0 : 1;
        }
    } else {
        Q_EMIT m_p->error(reply->errorString());
        m_errors |= Errors::URL;
        result = slotParseQuote(QString()) ? 0 : 1;
    }
    m_eventLoop->exit(result);
}

bool AlkOnlineQuote::Private::downloadUrl(const KUrl &url)
{
    QNetworkAccessManager manager(this);
    connect(&manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadUrlDone(QNetworkReply*)));

    QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("User-Agent", "alkimia " ALK_VERSION_STRING);
    manager.get(request);

    if (m_timeout != -1)
        QTimer::singleShot(m_timeout, this, SLOT(slotLoadTimeout()));
    m_eventLoop = new QEventLoop;
    int result = m_eventLoop->exec(QEventLoop::ExcludeUserInputEvents);
    delete m_eventLoop;
    m_eventLoop = nullptr;
    if (result == 2) {
        QNetworkRequest req;
        req.setUrl(m_url);
        req.setRawHeader("User-Agent", "alkimia " ALK_VERSION_STRING);
        manager.get(req);

        if (m_timeout != -1)
            QTimer::singleShot(m_timeout, this, SLOT(slotLoadTimeout()));
        m_eventLoop = new QEventLoop;
        result = m_eventLoop->exec(QEventLoop::ExcludeUserInputEvents);
        delete m_eventLoop;
        m_eventLoop = nullptr;
    }
    return result == 0;
}
#endif // BUILD_WITH_QTNETWORK

#ifdef ENABLE_FINANCEQUOTE
bool AlkOnlineQuote::Private::launchFinanceQuote(const QString &_symbol, const QString &_id,
                                        const QString &_sourcename)
{
    bool result = true;
    m_symbol = _symbol;
    m_id = _id;
    m_errors = Errors::None;
    m_source = AlkOnlineQuoteSource(_sourcename,
                                    m_profile->scriptPath(),
                                    "\"([^,\"]*)\",.*", // symbol regexp
                                    AlkOnlineQuoteSource::IdSelector::Symbol,
                                    "[^,]*,[^,]*,\"([^\"]*)\"", // price regexp
                                    "[^,]*,([^,]*),.*", // date regexp
                                    "%y-%m-%d"); // date format

    //Q_EMIT status(QString("(Debug) symbol=%1 id=%2...").arg(_symbol,_id));
    AlkFinanceQuoteProcess tmp;
    QString fQSource = m_profile->type() == AlkOnlineQuotesProfile::Type::Script ?
                tmp.crypticName(_sourcename) : _sourcename.section(' ', 1);

    QStringList args;
    args << "perl" << m_profile->scriptPath() << fQSource << m_symbol;
    m_filter.clearProgram();
    m_filter << args;
    Q_EMIT m_p->status(i18nc("Executing 'script' 'online source' 'investment symbol' ",
                      "Executing %1 %2 %3...", args.join(" "), QString(), QString()));

    m_filter.setOutputChannelMode(KProcess::MergedChannels);
    m_filter.start();

    // This seems to work best if we just block until done.
    if (m_filter.waitForFinished()) {
    } else {
        Q_EMIT m_p->error(i18n("Unable to launch: %1", m_profile->scriptPath()));
        m_errors |= Errors::Script;
        result = slotParseQuote(QString());
    }
    return result;
}
#endif

bool AlkOnlineQuote::Private::parsePrice(const QString &_pricestr)
{
    bool result = true;
    // not made static due to QRegExp
    const Regex nonDigitChar("\\D");
    const Regex validChars("^\\s*([0-9,.\\s]*[0-9,.])");

    if (validChars.hasRegexMatch(_pricestr)) {
        // Deal with european quotes that come back as X.XXX,XX or XX,XXX
        //
        // We will make the assumption that ALL prices have a decimal separator.
        // So "1,000" always means 1.0, not 1000.0.
        //

        // Remove all non-digits from the price string except the last one, and
        // set the last one to a period.
        QString pricestr = validChars.capturedText(_pricestr, 1);

        int pos = pricestr.lastIndexOf(Regex("\\D"));
        if (pos > 0) {
            pricestr[pos] = '.';
            pos = pricestr.lastIndexOf(Regex("\\D"), pos - 1);
        }
        while (pos > 0) {
            pricestr.remove(pos, 1);
            pos = pricestr.lastIndexOf(Regex("\\D"), pos);
        }

        bool ok;
        m_price = pricestr.toDouble(&ok);
        if (ok) {
            kDebug(Private::dbgArea()) << "Price" << pricestr;
            Q_EMIT m_p->status(i18n("Price found: '%1' (%2)", pricestr, m_price));
        } else {
            m_errors |= Errors::Price;
            Q_EMIT m_p->error(i18n("Price '%1' cannot be converted to a number for '%2'", pricestr, m_symbol));
            result = false;
        }
    } else {
        m_errors |= Errors::Price;
        Q_EMIT m_p->error(i18n("Unable to parse price for '%1'", m_symbol));
        result = false;
        m_price = 0.0;
    }
    return result;
}

bool AlkOnlineQuote::Private::parseDate(const QString &datestr)
{
    if (!datestr.isEmpty()) {
        Q_EMIT m_p->status(i18n("Date found: '%1'", datestr));

        AlkDateFormat dateparse(m_source.dateFormat());
        try {
            m_date = dateparse.convertString(datestr, false /*strict*/);
            kDebug(Private::dbgArea()) << "Date" << datestr;
            Q_EMIT m_p->status(i18n("Date format found: '%1' -> '%2'", datestr, m_date.toString()));
        } catch (const AlkException &e) {
            m_errors |= Errors::DateFormat;
            Q_EMIT m_p->error(i18n("Unable to parse date '%1' using format '%2': %3", datestr,
                                                                               dateparse.format(),
                                                                               e.what()));
            m_date = QDate::currentDate();
            Q_EMIT m_p->status(i18n("Using current date for '%1'", m_symbol));
        }
    } else {
        if (m_source.dateRegex().isEmpty()) {
            Q_EMIT m_p->status(i18n("Parsing date is disabled for '%1'", m_symbol));
        } else {
            m_errors |= Errors::Date;
            Q_EMIT m_p->error(i18n("Unable to parse date for '%1'", m_symbol));
        }
        m_date = QDate::currentDate();
        Q_EMIT m_p->status(i18n("Using current date for '%1'", m_symbol));
    }
    return true;
}

/**
 * Parse quote data expected as stripped html
 *
 * @param quotedata quote data to parse
 * @return true parsing successful
 * @return false parsing unsuccessful
 */
bool AlkOnlineQuote::Private::parseQuoteStripHTML(const QString &_quotedata)
{
    QString quotedata = _quotedata;

    //
    // First, remove extraneous non-data elements
    //

    // HTML tags
    quotedata.remove(Regex("<[^>]*>"));

    // &...;'s
    quotedata.replace(Regex("&\\w+;"), " ");

    // Extra white space
    quotedata = quotedata.simplified();
    kDebug(Private::dbgArea()) << "stripped text" << quotedata;

    return parseQuoteHTML(quotedata);
}

/**
 * Parse quote data in html format
 *
 * @param quotedata quote data to parse
 * @return true parsing successful
 * @return false parsing unsuccessful
 */
bool AlkOnlineQuote::Private::parseQuoteHTML(const QString &quotedata)
{
    bool gotprice = false;
    bool gotdate = false;
    bool result = true;

    Regex identifierRegExp(m_source.idRegex());
    Regex dateRegExp(m_source.dateRegex());
    Regex priceRegExp(m_source.priceRegex());

#if QT_VERSION < QT_VERSION_CHECK(5,0,0)

    if (identifierRegExp.indexIn(quotedata) > -1) {
        kDebug(Private::dbgArea()) << "Symbol" << identifierRegExp.cap(1);
        Q_EMIT m_p->status(i18n("Symbol found: '%1'", identifierRegExp.cap(1)));
    } else {
        m_errors |= Errors::Symbol;
        Q_EMIT m_p->error(i18n("Unable to parse symbol for %1", m_symbol));
    }

    if (priceRegExp.indexIn(quotedata) > -1) {
        QString pricestr = priceRegExp.cap(1);
        gotprice = parsePrice(pricestr);
    } else {
        gotprice = parsePrice(QString());
    }

    if (dateRegExp.indexIn(quotedata) > -1) {
        QString datestr = dateRegExp.cap(1);
        gotdate = parseDate(datestr);
    } else {
        gotdate = parseDate(QString());
    }

#else

    QRegularExpressionMatch match;
    match = identifierRegExp.match(quotedata);
    if (match.hasMatch()) {
        kDebug(Private::dbgArea()) << "Symbol" << match.captured(1);
        Q_EMIT m_p->status(i18n("Symbol found: '%1'", match.captured(1)));
    } else {
        m_errors |= Errors::Symbol;
        Q_EMIT m_p->error(i18n("Unable to parse symbol for %1", m_symbol));
    }

    match = priceRegExp.match(quotedata);
    if (match.hasMatch()) {
        QString pricestr = match.captured(1);
        gotprice = parsePrice(pricestr);
    } else {
        gotprice = parsePrice(QString());
    }

    match = dateRegExp.match(quotedata);
    if (match.hasMatch()) {
        QString datestr = match.captured(1);
        gotdate = parseDate(datestr);
    } else {
        gotdate = parseDate(QString());
    }

#endif

    if (gotprice && gotdate) {
        Q_EMIT m_p->quote(m_id, m_symbol, m_date, m_price);
    } else {
        Q_EMIT m_p->failed(m_id, m_symbol);
        result = false;
    }
    return result;
}

/**
 * Parse quote data in csv format
 *
 * @param quotedata quote data to parse
 * @return true parsing successful
 * @return false parsing unsuccessful
 */
bool AlkOnlineQuote::Private::parseQuoteCSV(const QString &quotedata)
{
    QString dateColumn(m_source.dateRegex());
    QString priceColumn(m_source.priceRegex());
    QStringList lines = quotedata.split(QRegExp("\r?\n"));
    QString header = lines.first();
    QString columnSeparator;
    Regex rx("([,;\t])");

#if QT_VERSION < QT_VERSION_CHECK(5,0,0)

    if (rx.indexIn(header) != -1) {
        columnSeparator = rx.cap(1);
    }

#else
    const auto match = rx.match(header);
    if (match.hasMatch()) {
        columnSeparator = match.captured(1);
    }

#endif

    if (columnSeparator.isEmpty()) {
        m_errors |= Errors::Source;
        Q_EMIT m_p->error(i18n("Unable to detect field delimiter in first line (header line) of quote data."));
        Q_EMIT m_p->failed(m_id, m_symbol);
        return false;
    }
    const QChar decimalSeparator = (columnSeparator.at(0) == ';') ? QLatin1Char(',') : QLatin1Char('.');

    // detect column index
    int dateCol = -1;
    int priceCol = -1;
    // check if column numbers are given
    if (dateColumn.startsWith(QLatin1Char('#')) && priceColumn.startsWith(QLatin1Char('#'))) {
        dateColumn.remove(0,1);
        priceColumn.remove(0,1);
        dateCol = dateColumn.toInt() - 1;
        priceCol = priceColumn.toInt() - 1;
    } else { // find columns
        QStringList headerColumns = header.split(columnSeparator);
        for (int i = 0; i < headerColumns.size(); i++) {
            if (headerColumns[i].contains(dateColumn))
                dateCol = i;
            else if (headerColumns[i].contains(priceColumn))
                priceCol = i;
        }
        lines.takeFirst();
    }
    if (dateCol == -1) {
        m_errors |= Errors::Date;
        Q_EMIT m_p->error(i18n("Unable to find date column '%1' in quote data", dateColumn));
        Q_EMIT m_p->failed(m_id, m_symbol);
        return false;
    }
    if (priceCol == -1) {
        m_errors |= Errors::Price;
        Q_EMIT m_p->error(i18n("Unable to find price column '%1' in quote data", priceColumn));
        Q_EMIT m_p->failed(m_id, m_symbol);
        return false;
    }
    AlkDatePriceMap prices;
    for (const auto &line : lines) {
        if (line.trimmed().isEmpty())
            continue;
        QStringList cols = line.split(columnSeparator);
        QString dateValue = cols[dateCol].trimmed();
        QString priceValue = cols[priceCol].trimmed();
        if (dateValue.isEmpty() || priceValue.isEmpty())
            continue;
        // @todo: auto detect format
        AlkDateFormat dateFormat(m_source.dateFormat());
        QDate date = dateFormat.convertString(dateValue, false);
        if (!date.isValid()) {
            m_errors |= Errors::DateFormat;
            Q_EMIT m_p->error(i18n("Unable to convert date '%1' with '%2' in quote data", dateValue, m_source.dateFormat()));
            Q_EMIT m_p->failed(m_id, m_symbol);
            return false;
        }
        if (!m_startDate.isNull() && date < m_startDate)
            continue;
        if (!m_endDate.isNull() && date > m_endDate)
            continue;
        prices[date] = AlkValue(priceValue, decimalSeparator);
    }
    if (prices.isEmpty()) {
        m_errors |= Errors::Price;
        Q_EMIT m_p->error(i18n("Unable to find date/price pairs in quote data"));
        Q_EMIT m_p->failed(m_id, m_symbol);
        return false;
    }

    if (m_useSingleQuoteSignal) {
        for (auto &key : prices.keys()) {
            Q_EMIT m_p->quote(m_id, m_symbol, key, prices[key].toDouble());
        }
    } else {
        Q_EMIT m_p->quotes(m_id, m_symbol, prices);
    }

    return true;
}

/**
 * Parse quote data according to currently selected web price quote source
 *
 * @param _quotedata quote data to parse
 * @return true parsing successful
 * @return false parsing unsuccessful
 */
bool AlkOnlineQuote::Private::slotParseQuote(const QString &quotedata)
{
    m_quoteData = quotedata;

    kDebug(Private::dbgArea()) << "quotedata" << quotedata;

    if (quotedata.isEmpty()) {
        m_errors |= Errors::Data;
        Q_EMIT m_p->error(i18n("Unable to update price for %1 (empty quote data)", m_symbol));
        Q_EMIT m_p->failed(m_id, m_symbol);
        return false;
    }
    switch (m_source.dataFormat()) {
    case AlkOnlineQuoteSource::StrippedHTML:
        return parseQuoteStripHTML(quotedata);
    case AlkOnlineQuoteSource::HTML:
        return parseQuoteHTML(quotedata);
    case AlkOnlineQuoteSource::CSV:
        return parseQuoteCSV(quotedata);
    default:
        return false;
    }
}

void AlkOnlineQuote::Private::slotLoadTimeout()
{
    Q_EMIT m_p->error(i18n("Timeout exceeded on fetching url for %1", m_symbol));
    m_errors |= Errors::Timeout;
    Q_EMIT m_p->failed(m_id, m_symbol);
    m_eventLoop->exit(Errors::Timeout);
}