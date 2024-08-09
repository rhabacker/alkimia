#ifndef ALKPAYEELINKSMODEL_H
#define ALKPAYEELINKSMODEL_H

#include "alkpayeereferencelink.h"

#include <QAbstractTableModel>

class AlkPayeeLinksModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Columns { Name, IdPattern, UrlTemplate, TestPattern, SearchReplacePattern, Uuid, Date };

    enum Roles { NameRole = Qt::UserRole };

    // explicit AlkPayeeLinksModel(AlkOnlineQuotesProfile *profile);
    explicit AlkPayeeLinksModel();

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    bool addRow(const AlkPayeeReferenceLinkData &link, const QModelIndex &parent = QModelIndex());

    // bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override
    // {
    //     beginInsertRows(parent, row, row + count - 1);

    //     endInsertRows();
    //     return true;
    // }

    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    QVector<AlkPayeeReferenceLinkData> &payeeLinks();

public Q_SLOTS:
    void slotSourcesChanged();

protected:
    QVector<AlkPayeeReferenceLinkData> _payeeLinks;
};

#endif // ALKPAYEELINKSMODEL_H
