/*
    SPDX-FileCopyrightText: 2018, 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef ALKWEBVIEW_H
#define ALKWEBVIEW_H

#include <alkimia/alk_export.h>

#if defined(BUILD_WITH_WEBENGINE)

#include <QWebEngineView>

class AlkWebPage;

/**
 *  The AlkWebView class provides a widget that
 *  is used to load and display web documents.
 *
 * @author Ralf Habacker ralf.habacker @freenet.de
 */
class ALK_EXPORT AlkWebView : public QWebEngineView
{
    Q_OBJECT
public:
    explicit AlkWebView(QWidget *parent = nullptr);
    virtual ~AlkWebView();

    void load(const QUrl &url);
    static void setWebInspectorEnabled(bool state);
    static bool webInspectorEnabled();
    AlkWebPage *webPage();
    void setWebPage(AlkWebPage *webPage);

Q_SIGNALS:
    void loadRedirectedTo(const QUrl &url);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
};

#elif defined(BUILD_WITH_WEBKIT)

#include <QWebView>

class AlkWebPage;

/**
 *  The AlkWebView class provides a widget that
 *  is used to load and display web documents.
 *
 * @author Ralf Habacker ralf.habacker @freenet.de
 */
class ALK_EXPORT AlkWebView : public QWebView
{
    Q_OBJECT
public:
    explicit AlkWebView(QWidget *parent = nullptr);
    virtual ~AlkWebView();

    void setWebInspectorEnabled(bool enable);
    bool webInspectorEnabled();
    AlkWebPage *webPage();
    void setWebPage(AlkWebPage *webPage);

Q_SIGNALS:
    void loadRedirectedTo(const QUrl &url);
};

#else

#include <QTextBrowser>

class AlkWebPage;

/**
 *  The AlkWebView class provides a widget that
 *  is used to load and display web documents.
 *
 * @author Ralf Habacker ralf.habacker @freenet.de
 */
class ALK_EXPORT AlkWebView : public QTextBrowser
{
    Q_OBJECT
public:
    explicit AlkWebView(QWidget *parent = nullptr);
    virtual ~AlkWebView();

    void load(const QUrl &url);
    void setHtml(const QString &data, const QUrl &baseUrl);
    void setUrl(const QUrl &url);
    void setWebInspectorEnabled(bool enable);
    bool webInspectorEnabled();
    AlkWebPage *webPage();
    void setWebPage(AlkWebPage *webPage);

Q_SIGNALS:
    void loadStarted();
    void loadFinished(bool);
    void loadRedirectedTo(const QUrl &to);

private:
    AlkWebPage *m_page{nullptr};
    QVariant loadResource(int type, const QUrl &name) override;
};
#endif

#endif // ALKWEBVIEW_H
