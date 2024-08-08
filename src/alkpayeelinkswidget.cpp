#include "alkpayeelinkswidget.h"
#include "kmessagebox.h"
#include "kstandardguiitem.h"

#include <klocalizedstring.h>

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QStringList>

class PayeeLink {
public:
    QString name;
    QString idPattern;
    QString urlTemplate;
    QString defaultValue;
};

Q_DECLARE_METATYPE(PayeeLink)

class AlkPayeeLinksModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Columns {
        Name,
        IdPattern,
        UrlTemplate
    };

    enum Roles {
        NameRole = Qt::UserRole
    };

    //explicit AlkPayeeLinksModel(AlkOnlineQuotesProfile *profile);
    explicit AlkPayeeLinksModel()
    {
    }

    int columnCount(const QModelIndex &parent = QModelIndex()) const override
    {
        return 3;
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override
    {
        return _payeeLinks.size();
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
    {
        if (!index.isValid())
            return QVariant();
        if (index.row() < 0 || index.row() >= rowCount())
            return QVariant();

        switch (role) {
            case Qt::DisplayRole:
            case Qt::EditRole:
                switch(index.column()) {
                case Name:
                    return _payeeLinks.at(index.row()).name;
                case IdPattern:
                    return _payeeLinks.at(index.row()).idPattern;
                case UrlTemplate:
                    return _payeeLinks.at(index.row()).urlTemplate;
                }
                break;
            case NameRole:
                return QVariant::fromValue(_payeeLinks.at(index.row()));
        }

        return QVariant();
    }

    Qt::ItemFlags flags(const QModelIndex &index) const override
    {
        if (!index.isValid())
            return Qt::NoItemFlags;
        if (index.row() < 0 || index.row() >= rowCount())
            return Qt::NoItemFlags;
        if (index.column() < 0 || index.column() >= columnCount())
            return Qt::NoItemFlags;

        Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled /*| QAbstractTableModel::flags(index)*/;
        switch (index.column()) {
            case Name: {
                QString name = _payeeLinks.at(index.row()).name;
                if (name.isEmpty())
                    return flags;
                flags |= Qt::ItemIsEditable;
                return flags;
            }
        }
        return flags;
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override
    {
        if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
            switch(section) {
            case Name: return i18n("Name");
            case IdPattern: return i18n("Id Pattern");
            case UrlTemplate: return i18n("Url Template");
            }
        }

        return QVariant();
    }

    bool addRow(const PayeeLink &link, const QModelIndex &parent = QModelIndex())
    {
        beginInsertRows(parent, _payeeLinks.size(), _payeeLinks.size());
        _payeeLinks.append(link);
        endInsertRows();
        return true;
    }

    // bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override
    // {
    //     beginInsertRows(parent, row, row + count - 1);

    //     endInsertRows();
    //     return true;
    // }

    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override
    {
        if (row < _payeeLinks.size()) {
            beginRemoveRows(parent, row, row + count - 1);
            _payeeLinks.remove(row, count);
            endRemoveRows();
            return true;
        }
        return false;
    }

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override
    {
        if(!index.isValid()) {
            return false;
        }
        if (index.row() < 0 || index.row() >= rowCount())
            return false;

        if (role == Qt::EditRole) {
            switch(index.column()) {
            case Name:
                _payeeLinks[index.row()].name = value.toString();
                Q_EMIT dataChanged(index, index);
                return true;
            case IdPattern:
                _payeeLinks[index.row()].idPattern = value.toString();
                Q_EMIT dataChanged(index, index);
                return true;
            case UrlTemplate:
                _payeeLinks[index.row()].urlTemplate = value.toString();
                Q_EMIT dataChanged(index, index);
                return true;
            }
        }
        return false;
    }

    QVector<PayeeLink> &payeeLinks() { return _payeeLinks; }

public Q_SLOTS:
    void slotSourcesChanged()
    {
    }

protected:
    //AlkOnlineQuotesProfile *_profile;
    QVector<PayeeLink> _payeeLinks;
};

#include <ui_alkpayeelinkslist.h>

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
        PayeeLink l1 = { "Ebay", "\\d{11}", "https://www.ebay.de/itm/%1", "156276533721" };
        PayeeLink l2 = { "Amazon", "\\w\\d{2}-\\d{7}-\\d{7}\\b}", "https://www.amazon.de/gp/your-account/order-details/ref=ppx_yo_dt_b_order_details_o00?ie=UTF8&orderID=%1", "" };
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

        connect(m_addReferenceButton, SIGNAL(clicked()), this, SLOT(slotAddReferenceButton()));
        connect(m_deleteButton, SIGNAL(clicked()), this, SLOT(slotDeleteEntry()));
        connect(m_newButton, SIGNAL(clicked()), this, SLOT(slotNewEntry()));
        connect(m_resetButton, SIGNAL(clicked()), this, SLOT(slotResetList()));
        //connect(m_checkButton, SIGNAL(clicked()), this, SLOT(slotCheckEntry()));
        connect(m_deleteButton, SIGNAL(clicked()), this, SLOT(slotDeleteEntry()));
        connect(m_duplicateButton, SIGNAL(clicked()), this, SLOT(slotDuplicateEntry()));
        connect(m_installButton, SIGNAL(clicked()), this, SLOT(slotInstallEntries()));
        connect(m_uploadButton, SIGNAL(clicked()), this, SLOT(slotUploadEntry()));
    }

public Q_SLOTS:
    void slotAddReferenceButton()
    {}

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

        PayeeLink pl = m_model->data(m_sourcesList->currentIndex(), AlkPayeeLinksModel::NameRole).value<PayeeLink>();
        pl.name.append(".copy");
        m_model->addRow(pl);
    }

    void slotInstallEntries()
    {}

    void slotNewEntry()
    {
        PayeeLink pl;
        pl.name = "new entry";
        m_model->addRow(pl);
    }

    void slotResetList()
    {}

    void slotUploadEntry()
    {}


};

AlkPayeeLinksWidget::AlkPayeeLinksWidget(QWidget *parent)
    : QWidget{parent}
    , d(new Private(this))
{
}

#include "alkpayeelinkswidget.moc"
