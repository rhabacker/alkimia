/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alkpayeelinkswidget.h"

#include "alkpayeereferencelink.h"
#include "alkpayeelinksmodel.h"
#include "ui_alkpayeelinkslist.h"

#include <KMessageBox>
#include <kstandardguiitem.h>

#include <QSortFilterProxyModel>
#include <QUrl>
#include <QUuid>

class AlkPayeeLinksWidget::Private : public QWidget, public Ui::AlkPayeeLinksListWidget
{
    Q_OBJECT
public:
    AlkPayeeLinksModel *m_model;
    Private(AlkPayeeLinksWidget *parent)
        : QWidget(parent)
    {
        Ui::AlkPayeeLinksListWidget::setupUi(parent);

        m_model = new AlkPayeeLinksModel;
        AlkPayeeReferenceLinkData l1 = {"Ebay",
                                        "\\d+-\\d+-\\d+",
                                        "https://order.ebay.de/ord/show?orderId=%1#/",
                                        "156-2765-3721",
                                        "",
                                        QUuid::createUuid().toString(),
                                        QDate::currentDate()};
        AlkPayeeReferenceLinkData l2 = {"Amazon",
                                        "\\w\\d{2}[-\\.]\\d{7}[-\\.]\\d{7}\\b",
                                        "https://www.amazon.de/gp/your-account/order-details/ref=ppx_yo_dt_b_order_details_o00?ie=UTF8&orderID=%1",
                                        "D01-2520833-1995838",
                                        "s/[.]/-",
                                        QUuid::createUuid().toString(),
                                        QDate::currentDate()};
        m_model->payeeLinks().append(l1);
        m_model->payeeLinks().append(l2);
        auto proxyModel = new QSortFilterProxyModel(this);
        proxyModel->setSourceModel(m_model);
        m_sourcesList->setModel(proxyModel);

        m_addReferenceButton->setVisible(false);
        m_installButton->setVisible(false);
        m_uploadButton->setVisible(false);
        m_resetButton->setVisible(false);

        QFontMetrics fm(QApplication::font());
        const int rowHeight = fm.height();
        m_sourcesList->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        m_sourcesList->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
        m_sourcesList->verticalHeader()->setDefaultSectionSize(rowHeight);
        m_sourcesList->verticalHeader()->setVisible(false);
        m_sourcesList->setShowGrid(false);
        m_sourcesList->horizontalHeader()->setVisible(true);
        m_sourcesList->setSortingEnabled(true);
        m_sourcesList->setSelectionMode(QAbstractItemView::SingleSelection);
        m_sourcesList->setSelectionBehavior(QAbstractItemView::SelectRows);
        connect(m_sourcesList, SIGNAL(clicked(QModelIndex)), this, SLOT(slotLoadSource(QModelIndex)));

        m_checkResultLabel->setText(QString());

        connect(m_addReferenceButton, SIGNAL(clicked()), this, SLOT(slotAddReferenceButton()));
        connect(m_deleteButton, SIGNAL(clicked()), this, SLOT(slotDeleteEntry()));
        connect(m_newButton, SIGNAL(clicked()), this, SLOT(slotNewEntry()));
        connect(m_resetButton, SIGNAL(clicked()), this, SLOT(slotResetList()));
        connect(m_checkButton, SIGNAL(clicked()), this, SLOT(slotCheckEntry()));
        connect(m_deleteButton, SIGNAL(clicked()), this, SLOT(slotDeleteEntry()));
        connect(m_duplicateButton, SIGNAL(clicked()), this, SLOT(slotDuplicateEntry()));
        connect(m_installButton, SIGNAL(clicked()), this, SLOT(slotInstallEntries()));
        connect(m_uploadButton, SIGNAL(clicked()), this, SLOT(slotUploadEntry()));

        connect(m_checkIdPatternInput, SIGNAL(textChanged(QString)), this, SLOT(slotCheckEntry()));
    }

public Q_SLOTS:
    void slotCheckEntry()
    {
        AlkPayeeReferenceLink link(m_model->data(m_sourcesList->currentIndex(), AlkPayeeLinksModel::NameRole).value<AlkPayeeReferenceLinkData>());
        QUrl url = link.link(m_checkIdPatternInput->text());
        link.setupLabel(m_checkResultLabel, url);
    }

    void slotDeleteEntry()
    {
        if (!m_sourcesList->currentIndex().isValid())
            return;

        int ret = KMessageBox::warningContinueCancel(this,
                                                     i18n("Are you sure to delete this payee link ?"),
                                                     i18n("Delete payee link"),
                                                     KStandardGuiItem::cont(),
                                                     KStandardGuiItem::cancel(),
                                                     QString("DeletingPayeeLink"));
        if (ret == KMessageBox::Cancel) {
            return;
        }

        // keep this order to avoid deleting the wrong current item
        m_sourcesList->model()->removeRow(m_sourcesList->currentIndex().row());
    }

    void slotDuplicateEntry()
    {
        if (!m_sourcesList->currentIndex().isValid())
            return;

        AlkPayeeReferenceLinkData pl = m_model->data(m_sourcesList->currentIndex(), AlkPayeeLinksModel::NameRole).value<AlkPayeeReferenceLinkData>();
        pl.name.append(".copy");
        m_model->addRow(pl);
    }

    void slotInstallEntries()
    {
    }

    void slotLoadSource(const QModelIndex &index)
    {
        m_checkIdPatternInput->setText(m_model->data(index, AlkPayeeLinksModel::NameRole).value<AlkPayeeReferenceLinkData>().testPattern);
    }

    void slotNewEntry()
    {
        AlkPayeeReferenceLinkData pl;
        pl.name = "new entry";
        m_model->addRow(pl);
    }

    void slotResetList()
    {
    }

    void slotUploadEntry()
    {
    }
};

AlkPayeeLinksWidget::AlkPayeeLinksWidget(QWidget *parent)
    : QWidget{parent}
    , d(new Private(this))
{
}

#include "alkpayeelinkswidget.moc"
