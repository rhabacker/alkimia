/*
    SPDX-FileCopyrightText: 2018, 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef ALKWEBPAGE_H
#define ALKWEBPAGE_H

#include <alkimia/alk_export.h>

#include <QObject>

class QUrl;

class ALK_EXPORT AlkWebPage : public QObject
{
    Q_OBJECT
public:
    explicit AlkWebPage(QWidget *parent = nullptr);
    virtual ~AlkWebPage();

    virtual QWidget *widget() = 0;
    virtual void load(const QUrl &url, const QString &acceptLanguage) = 0;
    virtual QString toHtml() = 0;
    virtual void setContent(const QString &data) = 0;
    virtual void setHtml(const QString &data, const QUrl &url) = 0;
    virtual QStringList getAllElements(const QString &symbol) = 0;
    virtual QString getFirstElement(const QString &symbol) = 0;
    /// Set timeout [ms] for AlkWebPage::toHtml()
    void setTimeout(int timeout = -1);
    virtual void setWebInspectorEnabled(bool state) = 0;
    int timeout() const;
    virtual bool webInspectorEnabled() = 0;

protected:
    int m_timeout;
};

class AlkWebView: public QObject
{
    Q_OBJECT
public:
    explicit AlkWebView(AlkWebPage *parent);
};

#if defined(BUILD_WITH_WEBENGINE)

#include <QWebEnginePage>

class ALK_EXPORT AlkWebPage : public QWebEnginePage
{
    Q_OBJECT
public:
    explicit AlkWebPage(QWidget *parent = nullptr);
    virtual ~AlkWebPage();

    QWidget *widget();
    void load(const QUrl &url, const QString &acceptLanguage);
    QString toHtml();
    void setContent(const QString &s);
    QStringList getAllElements(const QString &symbol);
    QString getFirstElement(const QString &symbol);
    /// Set timeout [ms] for AlkWebPage::toHtml()
    void setTimeout(int timeout = -1);
    static void setWebInspectorEnabled(bool state);
    int timeout();
    static bool webInspectorEnabled();

private:
    class Private;
    Private *d;

    bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame) override;
};

#endif
#if defined(BUILD_WITH_WEBKIT)

/**
 * The AlkWebPage class provides an interface
 * to a browser component
 * It is used for fetching and showing web pages.
 *
 * @author Ralf Habacker ralf.habacker @freenet.de
 */
class ALK_EXPORT AlkWebPageWebKit : public AlkWebPage
{
    Q_OBJECT
public:
    explicit AlkWebPageWebKit(QWidget *parent = nullptr);
    virtual ~AlkWebPageWebKit();

    QWidget *widget() override;
    void load(const QUrl &url, const QString &acceptLanguage) override;
    QString toHtml() override;
    void setContent(const QString &content) override;
    void setHtml(const QString &data, const QUrl &url) override;
    QStringList getAllElements(const QString &symbol) override;
    QString getFirstElement(const QString &symbol) override;
    void setWebInspectorEnabled(bool enable) override;
    bool webInspectorEnabled() override;

Q_SIGNALS:
    void loadRedirectedTo(const QUrl &url);

private:
    class Private;
    Private *d;
};

class AlkWebViewWebKit : public AlkWebView
{
};

#endif
#ifdef BUILD_WITH_QTEXTBROWSER

#include <QTextBrowser>

/**
 * The AlkWebPage class provides an interface
 * to a browser component with javascript support
 * It is used for fetching and showing web pages.
 *
 * @author Ralf Habacker ralf.habacker @freenet.de
 */
class ALK_EXPORT AlkWebPage : public QTextBrowser
{
    Q_OBJECT
public:
    explicit AlkWebPage(QWidget *parent = nullptr);
    virtual ~AlkWebPage();

    QWidget *widget();
    void load(const QUrl &url, const QString &acceptLanguage);
    void setHtml(const QString &data, const QUrl &url = QUrl());
    void setUrl(const QUrl &url);
    void setContent(const QString &s);
    QStringList getAllElements(const QString &symbol);
    QString getFirstElement(const QString &symbol);
    void setTimeout(int timeout = -1) { Q_UNUSED(timeout) }
    void setWebInspectorEnabled(bool enable);
    int timeout() { return -1; }
    bool webInspectorEnabled();
Q_SIGNALS:
    void loadStarted();
    void loadFinished(bool);
    void loadRedirectedTo(const QUrl&);

private:
    class Private;
    Private *d;
    QVariant loadResource(int type, const QUrl &name) override;
};
#endif


class AlkWebPageFactory
{
    enum Type {
        WebKit,
        WebEngine,
        QTextBrowser
    };

    static AlkWebPage* create(Type type);
};

#endif // ALKWEBPAGE_H
