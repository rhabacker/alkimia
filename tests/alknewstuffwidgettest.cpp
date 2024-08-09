/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alknewstuffwidget.h"

#include "alkapplication.h"

#if defined(Q_OS_WIN)
#include <QNetworkProxyFactory>
#endif

int main(int argc, char *argv[])
{
    AlkApplication app(argc,argv);

    // this is needed by KNS3
    AlkAboutData aboutData(QStringLiteral("alknewstuffwidgettest"), QStringLiteral("Alkimia New Stuff Widget Test"), QStringLiteral("1.0.0"));
    aboutData.setOrganizationDomain("kde.org");
    AlkAboutData::setApplicationData(aboutData);

#if defined(Q_OS_WIN)
    QNetworkProxyFactory::setUseSystemConfiguration(true);
#endif

    bool showPayeeLinks = app.arguments().size() > 1 && app.arguments().at(1) == "--payee-links";
    QString configFile = showPayeeLinks ? KNSRC_DIR "/alkimia-payee-links.knsrc" : KNSRC_DIR "/alkimia-quotes.knsrc";
    AlkNewStuffWidget engine;
    engine.init(configFile);
    engine.showInstallDialog();
}
