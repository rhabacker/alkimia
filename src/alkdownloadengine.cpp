#include "alkdownloadengine.h"

#include "alkonlinequote.h"
#include "alkimia/alkversion.h"

#include <QEventLoop>
#include <QTimer>
#ifndef BUILD_WITH_QTNETWORK
#else
#include <QNetworkReply>
#include <QNetworkAccessManager>
#endif

class AlkDownloadEngine::Private : public QObject {
    Q_OBJECT
public:
    AlkOnlineQuote *m_p;
    int m_timeout;
    QEventLoop *m_eventLoop;
    QUrl m_url;
    AlkOnlineQuote::Errors m_errors;
    bool downloadUrl(const QUrl& url);

    Private(AlkOnlineQuote *p)
        : m_p(p)
        , m_timeout(0)
    {}

#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
    static int dbgArea()
    {
        static int s_area = KDebug::registerArea("Alkimia (AlkOnlineQuote)");
        return s_area;
    }
#else
    static int dbgArea() {return 0; }
#endif

#ifndef BUILD_WITH_QTNETWORK
    void downloadUrlDone(KJob* job);
#else
    void downloadUrlDone(QNetworkReply *reply);
#endif
};

#ifndef BUILD_WITH_QTNETWORK
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)

bool AlkDownloadEngine::Private::downloadUrl(const QUrl& url)
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

void AlkDownloadEngine::Private::downloadUrlDone(KJob* job)
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
void AlkDownloadEngine::Private::downloadUrlDone(KJob* job)
{
    Q_UNUSED(job);
}

bool AlkDownloadEngine::Private::downloadUrl(const KUrl& url)
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

void AlkDownloadEngine::Private::downloadUrlDone(QNetworkReply *reply)
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
            kDebug(dbgArea()) << "Downloaded data from" << reply->url();
            result = processDownloadedPage(QUrl(reply->url()), reply->readAll()) ? 0 : 1;
        }
    } else {
        Q_EMIT m_p->error(reply->errorString());
        m_errors |= AlkOnlineQuote::Errors::URL;
        result = slotParseQuote(QString()) ? 0 : 1;
    }
    m_eventLoop->exit(result);
}

bool AlkDownloadEngine::Private::downloadUrl(const QUrl &url)
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

AlkDownloadEngine::AlkDownloadEngine(AlkOnlineQuote *p, QObject *parent)
    : QObject{parent}
    , d(new Private(p))
{
}

#include "alkdownloadengine.moc"
