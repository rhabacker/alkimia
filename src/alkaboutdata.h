/*
    SPDX-FileCopyrightText: 2025 Ralf Habacker ralf.habacker@freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef ALKABOUTDATA_H
#define ALKABOUTDATA_H

#include <KAboutData>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#define LICENCE_GPL KAboutLicense::GPL
#define LicenseKey KAboutLicense::LicenseKey
#else
#define LICENCE_GPL KAboutData::License_GPL
#endif

/**
 * Wrapper for KDE provided about data
 */
class AlkAboutData : public KAboutData
{
public:
    AlkAboutData(const QString &componentName,
                 const QString &displayName,
                 const QString &version,
                 const QString &shortDescription,
                 enum LicenseKey licenseType,
                 const QString &copyrightStatement = QString(),
                 const QString &otherText = QString(),
                 const QString &homePageAddress = QString(),
                 const QString &bugAddress = QStringLiteral("submit@bugs.kde.org"));
    AlkAboutData &setOrganizationDomain(const QByteArray &domain);
    static void setApplicationData(const AlkAboutData &aboutData);
};

#endif // ALKABOUTDATA_H
