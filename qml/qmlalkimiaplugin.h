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
#ifndef QMLALKIMIAPLUGIN_H
#define QMLALKIMIAPLUGIN_H

#include <QtGlobal>
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QtQml>
#else
#include <QtDeclarative>
class QQmlExtensionPlugin : public QDeclarativeExtensionPlugin
{
};
#undef Q_PLUGIN_METADATA
#define Q_PLUGIN_METADATA(s)
#define Q_DECL_OVERRIDE
#endif

class QmlAlkimiaPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.alkimia")

public:
    void registerTypes(const char *uri) Q_DECL_OVERRIDE;
};

#endif // QMLALKIMIAPLUGIN_H
