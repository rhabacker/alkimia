#include "alkpayeelinks.h"

#include "alkimia/alkversion.h"

#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkProxyFactory>
#include <QTextCodec>

#include <KEncodingProber>

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    #include <QtDebug>
    #define kDebug(a) qDebug()
#else
    #include <KDebug>
#endif

class AlkPayeeLinks::Private : public QObject
{
    Q_OBJECT
public:
    QEventLoop *m_eventLoop{0};
    QUrl m_url;
    AlkPayeeLinkConfigList m_configs;

    Private(QObject *parent)
        : QObject(parent)
    {
        fetchLinks(QUrl("http://dev.kmymoney.org/payeelinks.csv"));
    }

#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
    static int dbgArea()
    {
        static int s_area = KDebug::registerArea("Alkimia (AlkOnlineQuote)");
        return s_area;
    }
#endif
    bool processDownloadedData(const QUrl& url, const QByteArray& page)
    {
        bool result = true;
        KEncodingProber prober(KEncodingProber::Universal);
        prober.feed(page);
        QTextCodec *codec = QTextCodec::codecForName(prober.encoding());
        if (!codec) {
          codec = QTextCodec::codecForLocale();
        }
        QString data = codec->toUnicode(page);
        int lineNr = 1;
        QStringList headers;
        int version;
        foreach(const QString &line, data.split("\n")) {
            if (lineNr == 1) {
                // # version 1
                QStringList temp = line.split(' ');
                version = temp[2].toInt();
            } else if (lineNr == 2) {
                headers = line.split(';');
            } else if (AlkPayeeLinkConfig::match(line)) {
                AlkPayeeLinkConfig item(line, version);
                if (item.valid())
                    m_configs.append(item);
                else
                    kDebug(dbgArea()) << "skipped incomplete payee link config" << line;
            } else {
                kDebug(dbgArea()) << "invalid line found" << line;
                result = false;
            }
            lineNr++;
        }
        return result;
    }

    bool fetchLinks(const QUrl &url)
    {
        QNetworkAccessManager manager(this);
        connect(&manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadUrlDone(QNetworkReply*)));

        QNetworkRequest request;
        request.setUrl(url);
        request.setRawHeader("User-Agent", "alkimia " ALK_VERSION_STRING);
        manager.get(request);
        m_eventLoop = new QEventLoop;
        int result = m_eventLoop->exec(QEventLoop::ExcludeUserInputEvents);
        delete m_eventLoop;
        m_eventLoop = nullptr;
        if (result == 2) {
            QNetworkRequest request;
            request.setUrl(m_url);
            request.setRawHeader("User-Agent", "alkimia " ALK_VERSION_STRING);
            manager.get(request);
            m_eventLoop = new QEventLoop;
            result = m_eventLoop->exec(QEventLoop::ExcludeUserInputEvents);
            delete m_eventLoop;
            m_eventLoop = nullptr;
        }
        return result == 0;
    }

public slots:
    void downloadUrlDone(QNetworkReply *reply)
    {
        int result = 0;
        if (reply->error() == QNetworkReply::NoError) {
            QUrl newUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
            if (!newUrl.isEmpty() && newUrl != reply->url()) {
                m_url = reply->url().resolved(newUrl);
                result = 2;
            } else {
                kDebug(dbgArea()) << "Downloaded data from" << reply->url();
                result = processDownloadedData(reply->url(), reply->readAll()) ? 0 : 1;
            }
        } else {
            kDebug(dbgArea()) << reply->errorString();
        }
        m_eventLoop->exit(result);
    }
};

AlkPayeeLinks::AlkPayeeLinks(QObject *parent)
    : d(new Private(this))
{
}

AlkPayeeLinks::~AlkPayeeLinks()
{
}

AlkPayeeLinkConfigList &AlkPayeeLinks::payeeLinks()
{
    return d->m_configs;
}

AlkPayeeLinkConfig &AlkPayeeLinks::payeeLink(int i) const
{
    if (i < 0 || i >= d->m_configs.size())
        AlkPayeeLinkConfig();
    return d->m_configs[i];
}

const AlkPayeeLinkConfig &AlkPayeeLinks::payeeLink(const QString &key) const
{
    foreach(const AlkPayeeLinkConfig &config, d->m_configs) {
        if (config.key == key)
            return config;
    }
    return AlkPayeeLinkConfig();
}

AlkPayeeLinks &AlkPayeeLinks::instance()
{
    static AlkPayeeLinks instance;
    return instance;
}

#include "alkpayeelinks.moc"
