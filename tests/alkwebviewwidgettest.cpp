/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alkdebug.h"
#include "alktestdefs.h"
#include "alkwebpage.h"
#include "alkwebview.h"

#include <QApplication>
#include <QCheckBox>
#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

class AlkWebViewTestDialog : public QDialog
{
    Q_OBJECT
public:
    AlkWebView *view;
    QLineEdit *urlEdit;
    QLabel *startedLabel;
    QLabel *finishedLabel;
    QLabel *redirectedLabel;
    QLabel *clickedLabel;
    QLabel *clickedUrlLabel;
    QCheckBox *openLinksCheckBox;

    AlkWebViewTestDialog()
    {
#if defined(ALKIMIA_WEBENGINE)
        AlkWebView::setWebInspectorEnabled(true);
#endif

        view = new AlkWebView;
        view->setWebPage(new AlkWebPage(view));

#if defined(ALKIMIA_WEBKIT)
        view->setWebInspectorEnabled(true);
#endif

        QGridLayout *layout = new QGridLayout;

        urlEdit = new QLineEdit;
        QPushButton *loadButton = new QPushButton("Load URL");
        QPushButton *loadTestRedirectButton = new QPushButton("Load URL with redirection");
        QPushButton *loadTestHtmlButton = new QPushButton("Load test HTML");

        startedLabel = new QLabel;
        finishedLabel = new QLabel;
        redirectedLabel = new QLabel;
        clickedLabel = new QLabel;
        clickedUrlLabel = new QLabel;
        openLinksCheckBox = new QCheckBox("Open links");

        openLinksCheckBox->setChecked(true);

        /*
         * AlkWebView exposes the same signals for all supported backends.
         *
         * Use the old SIGNAL/SLOT syntax here because loadStarted/loadFinished
         * are not available with the same interface on every backend.
         */
        connect(view, SIGNAL(loadRedirectedTo(QUrl)),
                this, SLOT(slotRedirectedTo(QUrl)));

        connect(view, SIGNAL(loadStarted()),
                this, SLOT(slotStarted()));

        connect(view, SIGNAL(loadFinished(bool)),
                this, SLOT(slotFinished(bool)));

        // this signal is passed from the page, so it must work too
        connect(view, SIGNAL(linkClicked(QUrl)),
                this, SLOT(slotLinkClicked(QUrl)));

        connect(loadButton, SIGNAL(pressed()),
                this, SLOT(slotPressed()));

        connect(loadTestRedirectButton, SIGNAL(pressed()),
                this, SLOT(slotLoadRedirectTest()));

        connect(loadTestHtmlButton, SIGNAL(pressed()),
                this, SLOT(slotLoadTestHtml()));

        connect(openLinksCheckBox, &QCheckBox::toggled,
                this, [this](bool enabled) {
                    view->setOpenLinks(enabled);
                });

        layout->addWidget(urlEdit, 0, 0, 1, 4);
        layout->addWidget(loadButton, 0, 4);
        layout->addWidget(loadTestRedirectButton, 0, 5);
        layout->addWidget(loadTestHtmlButton, 0, 6);

        layout->addWidget(view, 1, 0, 7, 4);

        layout->addWidget(new QLabel("Signals"), 1, 4, 1, 2);

        layout->addWidget(new QLabel("started:"), 2, 4);
        layout->addWidget(startedLabel, 2, 5);

        layout->addWidget(new QLabel("finished:"), 3, 4);
        layout->addWidget(finishedLabel, 3, 5);

        layout->addWidget(new QLabel("redirected:"), 4, 4);
        layout->addWidget(redirectedLabel, 4, 5);

        layout->addWidget(new QLabel("link clicked:"), 5, 4);
        layout->addWidget(clickedLabel, 5, 5);

        layout->addWidget(new QLabel("clicked URL:"), 6, 4);
        layout->addWidget(clickedUrlLabel, 6, 5);

        layout->addWidget(openLinksCheckBox, 7, 4, 1, 2);

        // Keep the signal labels at their natural height.
        layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), 8, 4, 1, 2);

        setLayout(layout);

        QString url = QLatin1String(TEST_DOWNLOAD_URL_CURRENCY) + "&redirect=1";
        urlEdit->setText(url);

        view->setOpenLinks(openLinksCheckBox->isChecked());
        view->load(url);
    }

public Q_SLOTS:
    void slotPressed()
    {
        QUrl url(urlEdit->text());

        resetStatus();

        if (url.isValid())
            view->load(url);
    }

    void slotLoadRedirectTest()
    {
        resetStatus();
        const QString url = QLatin1String(TEST_DOWNLOAD_URL_CURRENCY) + "&redirect=1";
        urlEdit->setText(url);
        view->load(url);
    }

    void slotLoadTestHtml()
    {
        resetStatus();

        /*
         * Keep this page self-contained.  This makes testing linkClicked()
         * and openLinks independent of network availability.
         *
         * The links deliberately point to external URLs.  When openLinks()
         * is disabled, clicking one must still emit linkClicked(), but must
         * not navigate away from this page.
         */
        const QString html = QStringLiteral(
            "<html>"
            "<head>"
            "<title>AlkWebView link test</title>"
            "</head>"
            "<body>"
            "<h1>AlkWebView link test</h1>"
            "<p>Click one of the following links:</p>"
            "<ul>"
            "<li><a href=\"https://www.example.com/\">Example.com</a></li>"
            "<li><a href=\"https://www.kde.org/\">KDE</a></li>"
            "<li><a href=\"file://teftfile/\">a local test file</a></li>"
            "</ul>"
            "<p>"
            "With <b>Open links</b> enabled, the browser navigates to the "
            "clicked URL."
            "</p>"
            "<p>"
            "With <b>Open links</b> disabled, the browser remains on this "
            "page while the <code>linkClicked</code> signal is still emitted."
            "</p>"
            "</body>"
            "</html>");

#if defined(ALKIMIA_WEBENGINE)
        /*
         * QWebEngine does not provide a setHtml() function through the
         * AlkWebView abstraction, so use the page directly.
         */
        view->webPage()->setHtml(html, QUrl(QStringLiteral("about:blank")));
#elif defined(ALKIMIA_WEBKIT)
        view->webPage()->setHtml(html);
#else
        view->setHtml(html, QUrl(QStringLiteral("about:blank")));
#endif
    }

    void slotStarted()
    {
        startedLabel->setText("ok");
    }

    void slotRedirectedTo(const QUrl &url)
    {
        if (url.isValid()) {
            redirectedLabel->setText("ok");
            urlEdit->setText(url.toString());
        }
    }

    void slotFinished(bool ok)
    {
        finishedLabel->setText(ok ? "ok" : "failed");
    }

    void slotLinkClicked(const QUrl &url)
    {
        clickedLabel->setText("ok");
        clickedUrlLabel->setText(url.toString());
    }

private:
    void resetStatus()
    {
        startedLabel->setText("");
        finishedLabel->setText("");
        redirectedLabel->setText("");
        clickedLabel->setText("");
        clickedUrlLabel->setText("");
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    AlkWebViewTestDialog dialog;
    dialog.resize(1000, 600);
    dialog.show();

    return app.exec();
}

#include "alkwebviewwidgettest.moc"
