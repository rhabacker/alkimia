/*
    SPDX-FileCopyrightText: 2025 Ralf Habacker ralf.habacker@freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "alkaboutdata.h"

#include "alkapplication.h"
#include "alklocale.h"

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include <KCmdLineArgs>
#endif

AlkAboutData::AlkAboutData(const QString &componentName,
                           const QString &displayName,
                           const QString &version,
                           const QString &shortDescription,
                           enum LicenseKey licenseType,
                           const QString &copyrightStatement,
                           const QString &otherText,
                           const QString &homePageAddress,
                           const QString &bugAddress)
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    : KAboutData(componentName, displayName, version, shortDescription, licenseType, copyrightStatement, otherText, homePageAddress, bugAddress)
{
    KLocalizedString::setApplicationDomain(componentName.toLatin1());
    AlkApplication::createInstance();
}
#else
    : KAboutData(componentName.toLatin1(),
                 componentName.toLatin1(),
                 ki18n(displayName.toLatin1()),
                 version.toLatin1(),
                 ki18n(shortDescription.toLatin1()),
                 licenseType,
                 ki18n(copyrightStatement.toLatin1()),
                 ki18n(otherText.toLatin1()),
                 homePageAddress.toLatin1(),
                 bugAddress.toLatin1())
{
    KCmdLineArgs::init(this);
    AlkApplication::createInstance();
}
#endif

AlkAboutData &AlkAboutData::setOrganizationDomain(const QByteArray &domain)
{
    KAboutData::setOrganizationDomain(domain);
    return *this;
}

void AlkAboutData::setApplicationData(const AlkAboutData &aboutData)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    KAboutData::setApplicationData(aboutData);
#else
#endif
}
