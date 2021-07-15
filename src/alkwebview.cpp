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

#include "alkwebview.h"

#include "alkwebpage.h"

#include <QPointer>

#if defined(BUILD_WITH_WEBENGINE)

class AlkWebView::Private : public QObject
{
    Q_OBJECT
public:
    AlkWebView *q;
    QPointer<AlkWebPage> page;

    Private(AlkWebView*_q)
        : q(_q)
        , page(nullptr)
        {}

    ~Private()
    {
    }

    void setWebInspectorEnabled(bool enable)
    {
        Q_UNUSED(enable)
    }

    bool webInspectorEnabled()
    {
        return false;
    }
};

AlkWebView::AlkWebView(AlkWebPage *page, QWidget *parent)
    : QWebEngineView(parent)
    , d(new Private(this))
{
    setPage(page);
}

AlkWebView::~AlkWebView()
{
    delete d;
}

AlkWebPage *AlkWebView::page()
{
    return d->page;
}

void AlkWebView::setPage(AlkWebPage *page)
{
    page->setView(this);
    QWebEngineView::setPage(page);
    d->page = page;
}

void AlkWebView::setWebInspectorEnabled(bool enable)
{
    d->setWebInspectorEnabled(enable);
}

bool AlkWebView::webInspectorEnabled()
{
    return d->webInspectorEnabled();
}

#elif defined(BUILD_WITH_WEBKIT)

#include <QWebInspector>
#include <QWebView>

class AlkWebView::Private : public QObject
{
    Q_OBJECT
public:
    AlkWebView *q;
    QPointer<AlkWebPage> page;
    QWebInspector *inspector;

    Private(AlkWebView*_q)
        : q(_q)
        , page(nullptr)
        , inspector(nullptr)
        {}

    ~Private()
    {
        page->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, false);
        inspector->setPage(nullptr);
        delete inspector;
    }

    void setWebInspectorEnabled(bool enable)
    {
        page->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, enable);
        if (enable && !inspector) {
            inspector = new QWebInspector();
            inspector->setPage(page);
        }
    }

    bool webInspectorEnabled()
    {
        return page->settings()->testAttribute(QWebSettings::DeveloperExtrasEnabled);
    }
};

AlkWebView::AlkWebView(AlkWebPage *page, QWidget *parent)
    : QWebView(parent)
    , d(new Private(this))
{
    setPage(page);
}

AlkWebView::~AlkWebView()
{
    delete d;
}

AlkWebPage *AlkWebView::page()
{
    return d->page;
}

void AlkWebView::setPage(AlkWebPage *page)
{
    page->setView(this);
    QWebView::setPage(page);
    d->page = page;
}

void AlkWebView::setWebInspectorEnabled(bool enable)
{
    d->setWebInspectorEnabled(enable);
}

bool AlkWebView::webInspectorEnabled()
{
    return d->webInspectorEnabled();
}

#else

class AlkWebView::Private : public QObject
{
    Q_OBJECT
public:
    AlkWebView *q;
    QPointer<AlkWebPage> page;

    Private(AlkWebView*_q)
        : q(_q)
        , page(nullptr)
        {}

    ~Private()
    {
    }

    void setWebInspectorEnabled(bool enable)
    {
        Q_UNUSED(enable)
    }

    bool webInspectorEnabled()
    {
        return false;
    }
};

AlkWebView::AlkWebView(AlkWebPage *page, QWidget *parent)
    : d(new Private(this))
{
    Q_UNUSED(parent)
    setPage(page);
}

AlkWebView::~AlkWebView()
{
    delete d;
}

AlkWebPage *AlkWebView::page()
{
    return d->page;
}

void AlkWebView::setPage(AlkWebPage *page)
{
    d->page = page;
}

void AlkWebView::setWebInspectorEnabled(bool enable)
{
    d->setWebInspectorEnabled(enable);
}

bool AlkWebView::webInspectorEnabled()
{
    return d->webInspectorEnabled();
}

#endif

#include "alkwebview.moc"
