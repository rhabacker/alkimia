#include "alkpayeelinkswidget.h"
#include "ui_alkpayeelinkswidgetimpl.h"

#include "alkpayeelinks.h"

class AlkPayeeLinksWidget::Private : public QWidget, public Ui::AlkPayeeLinksWidgetImpl
{
    Q_OBJECT
public:
    Private(QWidget *parent);

public slots:
    void slotMatchingCheck(const QString &text);
};

AlkPayeeLinksWidget::Private::Private(QWidget *parent)
{
    setupUi(parent);
    m_payeeLinkSource->addItem(i18n("Disabled"));
    foreach(const AlkPayeeLinkConfig &config, AlkPayeeLinks::instance().payeeLinks()) {
        m_payeeLinkSource->addItem(config.name, QVariant(config.key));
    }
    m_customMatchSettings->setEnabled(false);
    connect(m_useCustomSource, SIGNAL(clicked(bool)), m_customMatchSettings, SLOT(setEnabled(bool)));
    connect(m_useCustomSource, SIGNAL(clicked(bool)), m_payeeLinkSource, SLOT(setDisabled(bool)));
    connect(m_identifierCheckEdit, SIGNAL(textChanged(QString)), this, SLOT(slotMatchingCheck(QString)));
}

void AlkPayeeLinksWidget::Private::slotMatchingCheck(const QString &text)
{
    m_capturedResult->setText("");
    m_resultingURL->setText("");
    AlkPayeeLinkConfig config;
    if (m_useCustomSource->checkState() == Qt::Checked) {
        if (m_idPatternEdit->text().isEmpty() || m_idPatternEdit->text().isEmpty())
            return;
        config.idPattern = m_idPatternEdit->text();
        config.urlTemplate = m_urlTemplateEdit->text();
    } else {
        QString key = m_payeeLinkSource->currentData().toString();
        if (key.isEmpty())
            return;
        config = AlkPayeeLinks::instance().payeeLink(key);
    }
    QStringList matches = config.matchingLinks(m_identifierCheckEdit->text());
    if (matches.size() > 0) {
        QUrl url = config.payeeLink(m_identifierCheckEdit->text());
        m_capturedResult->setText(matches[0]);
        m_resultingURL->setTextInteractionFlags(Qt::TextBrowserInteraction);
        m_resultingURL->setText(QString("<a href=\"%1\">%1</a>").arg(url.toString(), url.toString()));
    }
}


AlkPayeeLinksWidget::AlkPayeeLinksWidget(QWidget *parent)
    : QWidget(parent)
    , d(new Private(this))
{
}

AlkPayeeLinksWidget::~AlkPayeeLinksWidget()
{
    delete d;
}

#include "alkpayeelinkswidget.moc"
