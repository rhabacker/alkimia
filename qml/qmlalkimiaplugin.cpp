/***************************************************************************
 *   Copyright 2018  Ralf Habacker ralf.habacker@freenet.de                *
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

#include "qmlalkimiaplugin.h"

#include "alkonlinequote.h"

void QmlAlkimiaPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("org.kde.alkimia"));
    qmlRegisterType<AlkOnlineQuote>(uri, 1, 0, "AlkOnlineQuote");
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(qmlalkimiaplugin, QmlAlkimiaPlugin);
#endif