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

AlkOnlineQuotesProfileManager manager;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    manager.addProfile(new AlkOnlineQuotesProfile("onlinequoteseditor", AlkOnlineQuotesProfile::Type::GHNS, "skrooge_unit.knsrc"));
    AlkOnlineQuoteSource::setProfile(manager.profiles().first());
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
