/***************************************************************************
 *   Copyright 2020 Ralf Habacker <ralf.habacker@freenet.de>               *
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

#ifndef ALKWEBVIEW_H
#define ALKWEBVIEW_H

#include <alkimia/alk_export.h>

#include <QWidget>

class AlkWebPage;

#if defined(BUILD_WITH_WEBENGINE)

#include <QWebEngineView>

class ALK_EXPORT AlkWebView : public QWebEngineView
{
    Q_OBJECT
public:
    AlkWebView(AlkWebPage *page, QWidget *parent = nullptr);
    virtual ~AlkWebView();

    AlkWebPage *page();
    void setPage(AlkWebPage *page);
    void setWebInspectorEnabled(bool state);
    bool webInspectorEnabled();

private:
    class Private;
    Private *d;
};

#elif defined(BUILD_WITH_WEBKIT)

#include <QWebView>

/**
 * The AlkWebView class provides an interface
 * to a browser component with javascript support
 * It is used for fetching and showing web pages.
 *
 * @author Ralf Habacker <ralf.habacker@freenet.de>
 */
class ALK_EXPORT AlkWebView : public QWebView
{
    Q_OBJECT
public:
    AlkWebView(AlkWebPage *page, QWidget *parent = nullptr);
    virtual ~AlkWebView();

    AlkWebPage *page();
    void setPage(AlkWebPage *page);
    void setWebInspectorEnabled(bool enable);
    bool webInspectorEnabled();

private:
    class Private;
    Private *d;
};

#else

#include <QWidget>

/**
 * The AlkWebView class provides an interface
 * to a browser component with javascript support
 * It is used for fetching and showing web pages.
 *
 * @author Ralf Habacker <ralf.habacker@freenet.de>
 */
class ALK_EXPORT AlkWebView : public QWidget
{
    Q_OBJECT
public:
    AlkWebView(AlkWebPage *page, QWidget *parent = nullptr);
    virtual ~AlkWebView();

    AlkWebPage *page();
    void setPage(AlkWebPage *page);
    void setWebInspectorEnabled(bool enable);
    bool webInspectorEnabled();

private:
    class Private;
    Private *d;
};
#endif

#endif // ALKWEBVIEW_H
