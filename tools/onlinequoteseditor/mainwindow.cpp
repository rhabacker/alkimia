/***************************************************************************
 *   Copyright 2018 Ralf Habacker <ralf.habacker@freenet.de>               *
 *                                                                         *
 *   This file is part of libalkimia.                                      *
 *                                                                         *
 *   libalkimia is free software; you can redistribute it and/or           *
 *   modify it under the terms of the GNU General Public License           *
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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "alkonlinequotesprofilemanager.h"
#include "alkonlinequoteswidget.h"

#include <QDockWidget>
#include <QWebInspector>

AlkOnlineQuotesProfileManager manager;

class MainWindow::Private : public Ui::MainWindow
{
public:
    Private(QMainWindow *parent)
    {
        setupUi(parent);
    }
};

void MainWindow::slotUrlChanged(const QUrl &url)
{
    d->urlLine->setText(url.toString());
}

void MainWindow::slotEditingFinished()
{
    d->webView->setUrl(QUrl(d->urlLine->text()));
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    d(new Private(this))
{
    manager.addProfile(new AlkOnlineQuotesProfile("onlinequoteseditor", AlkOnlineQuotesProfile::Type::GHNS, "skrooge_unit.knsrc"));
    //manager.addProfile(new AlkOnlineQuotesProfile("local", AlkOnlineQuotesProfile::Type::GHNS, "skrooge_unit_local.knsrc"));
    //manager.addProfile(new AlkOnlineQuotesProfile("skrooge", AlkOnlineQuotesProfile::Type::GHNS, "skrooge_unit.knsrc"));
    //manager.addProfile(new AlkOnlineQuotesProfile("kmymoney", AlkOnlineQuotesProfile::Type::KMymoney));
    AlkOnlineQuoteSource::setProfile(manager.profiles().first());
    d->quotesWidget->init();
    d->progressBar->setVisible(true);
    connect(d->webView, SIGNAL(urlChanged(QUrl)), this, SLOT(slotUrlChanged(QUrl)));
    connect(d->webView, SIGNAL(loadProgress(int)), d->progressBar, SLOT(setValue(int)));
    connect(d->urlLine, SIGNAL(editingFinished()), this, SLOT(slotEditingFinished()));
    QUrl url("https://support.cegit.sag.de");
    d->webView->setUrl(url);

    // setup inspector
    d->webView->page()->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    QWebInspector *inspector = new QWebInspector;
    inspector->setPage(d->webView->page());

    d->quotesWidget->setView(d->webView);
}

MainWindow::~MainWindow()
{
    delete d;
}

