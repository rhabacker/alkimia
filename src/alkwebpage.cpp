/***************************************************************************
 *   Copyright 2018 Ralf Habacker <ralf.habacker@freenet.de>               *
 *                                                                         *
 *   This file is part of libalkimia.                                      *
 *                                                                         *
 *   libalkimia is free software; you can redistribute it and/or           *
 *   modify it under the terms of the GNU Lesser General Public License    *
 *   as published by the Free Software Foundation; either version 2.1 of   *
 *   the License or (at your option) version 3 or any later version.       *
 *                                                                         *
 *   libalkimia is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>  *
 ***************************************************************************/

#include "alkwebpage.h"

#if defined(BUILD_WITH_WEBENGINE)
#include <QEventLoop>
#include <QWebEnginePage>
#include <QUrl>

class AlkWebPage::Private : public QObject
{
    Q_OBJECT
public:
    AlkWebPage *q;

    Private(AlkWebPage *_q) : q(_q) {}
    void slotUrlChanged(const QUrl &url)
    {
        // This workaround is necessary because QWebEnginePage::urlChanged()
        // returns the html content set with setContent() as url.
        if (url.scheme().startsWith("http"))
            emit q->urlChanged(url);
        else
            emit q->urlChanged(QUrl());
    }
};

AlkWebPage::AlkWebPage(QObject *parent)
  : QWebEnginePage(parent)
  , d(new Private(this))
{
    connect(this, &QWebEnginePage::urlChanged, d, &Private::slotUrlChanged);
}

AlkWebPage::~AlkWebPage()
{
    delete d;
}

void AlkWebPage::load(const QUrl &url, const QString &acceptLanguage)
{
    Q_UNUSED(acceptLanguage)

    setUrl(url);
}

void AlkWebPage::setContent(const QByteArray &data, const QString &mimeType, const QUrl &baseUrl)
{
    Q_UNUSED(data)
    Q_UNUSED(mimeType)
    Q_UNUSED(baseUrl)

    qWarning() << "not implemented yet";
}

void AlkWebPage::setUrl(const QUrl &url)
{
    Q_UNUSED(url)

    qWarning() << "not implemented yet";
}

QString AlkWebPage::toHtml()
{
    QString html;
    QEventLoop loop;
    QWebEnginePage::toHtml([&html, &loop](const QString &result)
        {
            html = result;
            loop.quit();
        }
    );
    loop.exec();
    return html;
}

QString AlkWebPage::getFirstElement(const QString &symbol)
{
    Q_UNUSED(symbol)

    return QString();
}

#include "alkwebpage.moc"

#elif defined(BUILD_WITH_WEBKIT)
#include <QWebFrame>
#include <QWebElement>
#include <QNetworkRequest>

class AlkWebPage::Private
{
public:
    AlkWebPage *p;
    QNetworkAccessManager *networkAccessManager;
    Private(AlkWebPage *parent)
      : p(parent)
      , networkAccessManager(new QNetworkAccessManager)
    {
#if QT_VERSION >= QT_VERSION_CHECK(5,9,0)
        // see https://community.kde.org/Policies/API_to_Avoid#QNetworkAccessManager
        networkAccessManager->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
#endif
        p->setNetworkAccessManager(networkAccessManager);
    }

    ~Private()
    {
    }

};

AlkWebPage::AlkWebPage(QObject *parent)
  : QWebPage(parent)
  , d(new Private(this))
{
    settings()->setAttribute(QWebSettings::JavaEnabled, false);
    settings()->setAttribute(QWebSettings::AutoLoadImages, false);
    settings()->setAttribute(QWebSettings::PluginsEnabled, false);
}

AlkWebPage::~AlkWebPage()
{
    delete d;
}

void AlkWebPage::load(const QUrl &url, const QString &acceptLanguage)
{
    QNetworkRequest request;
    request.setUrl(url);
    if (!acceptLanguage.isEmpty())
        request.setRawHeader("Accept-Language", acceptLanguage.toLocal8Bit());
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    if (url.query().toLower().contains(QLatin1String("method=post"))) {
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	mainFrame()->load(request, QNetworkAccessManager::PostOperation, url.query().toUtf8());
#else
    if (url.hasQueryItem(QLatin1String("method")) && url.queryItemValue(QLatin1String("method")).toLower()== QLatin1String("post")) {
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        mainFrame()->load(request, QNetworkAccessManager::PostOperation);
#endif
    } else
    mainFrame()->load(request);
}

void AlkWebPage::setContent(const QByteArray &data, const QString &mimeType, const QUrl &baseUrl)
{
    mainFrame()->setContent(data, mimeType, baseUrl);
}

void AlkWebPage::setUrl(const QUrl &url)
{
    mainFrame()->setUrl(url);
}

QString AlkWebPage::toHtml()
{
    return mainFrame()->toHtml();
}

QString AlkWebPage::getFirstElement(const QString &symbol)
{
    QWebElement element = mainFrame()->findFirstElement(symbol);
    return element.toPlainText();
}
#else

class AlkWebPage::Private
{
public:
};

AlkWebPage::AlkWebPage(QObject *parent)
  : QObject(parent)
  , d(new Private)
{
}

AlkWebPage::~AlkWebPage()
{
    delete d;
}

void AlkWebPage::load(const QUrl &url, const QString &acceptLanguage)
{
    Q_UNUSED(url)
    Q_UNUSED(acceptLanguage)
}

void AlkWebPage::setUrl(const QUrl &url)
{
    Q_UNUSED(url)
}

void AlkWebPage::setContent(const QByteArray& data, const QString& mimeType, const QUrl& baseUrl)
{
    Q_UNUSED(data)
    Q_UNUSED(mimeType)
    Q_UNUSED(baseUrl)
}

QString AlkWebPage::toHtml()
{
    return QString();
}

QString AlkWebPage::getFirstElement(const QString &symbol)
{
    Q_UNUSED(symbol)

    return QString();
}
#endif
