/*
    SPDX-FileCopyrightText: 2023 Ralf Habacker  ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef AlkOnlineQuoteUploadDialog_H
#define AlkOnlineQuoteUploadDialog_H

#include <QDialog>
#include <QUrl>

class AlkOnlineQuoteSource;

namespace Ui {
class AlkOnlineQuoteUploadDialog;
}

class AlkOnlineQuoteUploadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AlkOnlineQuoteUploadDialog( const AlkOnlineQuoteSource &source, QWidget *parent = nullptr);
    ~AlkOnlineQuoteUploadDialog();

    static bool isSupported();
    bool compressFile(const QString &outFile, const QString &inFile);

private Q_SLOTS:
    void slotCreatePackage();
    void slotCopyToClipboard();
    void slotEnterStore();

private:
    Ui::AlkOnlineQuoteUploadDialog *ui;
    const AlkOnlineQuoteSource &m_source;
    QUrl m_file;
};

#endif // AlkOnlineQuoteUploadDialog_H
