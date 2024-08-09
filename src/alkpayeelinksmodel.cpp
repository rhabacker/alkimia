#include "alkpayeelinksmodel.h"
#include "klocalizedstring.h"

AlkPayeeLinksModel::AlkPayeeLinksModel()
{
}

int AlkPayeeLinksModel::columnCount(const QModelIndex &) const
{
    return 6;
}

int AlkPayeeLinksModel::rowCount(const QModelIndex &) const
{
    return _payeeLinks.size();
}

QVariant AlkPayeeLinksModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (index.row() < 0 || index.row() >= rowCount())
        return QVariant();

    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        switch (index.column()) {
        case Name:
            return _payeeLinks.at(index.row()).name;
        case IdPattern:
            return _payeeLinks.at(index.row()).idPattern;
        case UrlTemplate:
            return _payeeLinks.at(index.row()).urlTemplate;
        case TestPattern:
            return _payeeLinks.at(index.row()).testPattern;
        case SearchReplacePattern:
            return _payeeLinks.at(index.row()).searchReplacePattern;
        case Uuid:
            return _payeeLinks.at(index.row()).uuid;
        case Date:
            return _payeeLinks.at(index.row()).date;
        }
        break;
    case NameRole:
        return QVariant::fromValue(_payeeLinks.at(index.row()));
    }

    return QVariant();
}

Qt::ItemFlags AlkPayeeLinksModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    if (index.row() < 0 || index.row() >= rowCount())
        return Qt::NoItemFlags;
    if (index.column() < 0 || index.column() >= columnCount())
        return Qt::NoItemFlags;

    Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled /*| QAbstractTableModel::flags(index)*/;
    switch (index.column()) {
    case Name:
    case IdPattern:
    case UrlTemplate:
    case TestPattern:
        flags |= Qt::ItemIsEditable;
        return flags;
    }
    return flags;
}

QVariant AlkPayeeLinksModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal)
        switch (role) {
        case Qt::DisplayRole:
            switch (section) {
            case Name:
                return i18n("Name");
            case IdPattern:
                return i18n("Search pattern");
            case UrlTemplate:
                return i18n("Url template");
            case TestPattern:
                return i18n("Test pattern");
            case SearchReplacePattern:
                return i18n("Search/Replace pattern");
            case Uuid:
                return i18n("Unique identifier");
            case Date:
                return i18n("Date");
            }
            break;
        case Qt::ToolTipRole:
            switch (section) {
            case Name:
                return i18n("Name of the link source");
            case IdPattern:
                return i18n("Regular expression for finding the identifier");
            case UrlTemplate:
                return i18n("Template for the remote url, use %1 as placeholder");
            case TestPattern:
                return i18n("Pattern used as default value for testing");
            case SearchReplacePattern:
                return i18n("Pattern used to search and replace characters in generated urls");
            case Uuid:
                return i18n("Unique identifier for ...");
            case Date:
                return i18n("Valid from date");
            }
            break;
        }
    return QVariant();
}

bool AlkPayeeLinksModel::addRow(const AlkPayeeReferenceLinkData &link, const QModelIndex &parent)
{
    beginInsertRows(parent, _payeeLinks.size(), _payeeLinks.size());
    _payeeLinks.append(link);
    endInsertRows();
    return true;
}

bool AlkPayeeLinksModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (row < _payeeLinks.size()) {
        beginRemoveRows(parent, row, row + count - 1);
        _payeeLinks.remove(row, count);
        endRemoveRows();
        return true;
    }
    return false;
}

bool AlkPayeeLinksModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid()) {
        return false;
    }
    if (index.row() < 0 || index.row() >= rowCount())
        return false;

    if (role == Qt::EditRole) {
        switch (index.column()) {
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
        case TestPattern:
            _payeeLinks[index.row()].testPattern = value.toString();
            Q_EMIT dataChanged(index, index);
            return true;
        case SearchReplacePattern:
            _payeeLinks[index.row()].searchReplacePattern = value.toString();
            Q_EMIT dataChanged(index, index);
            return true;
        }
    }
    return false;
}

QVector<AlkPayeeReferenceLinkData> &AlkPayeeLinksModel::payeeLinks()
{
    return _payeeLinks;
}

void AlkPayeeLinksModel::slotSourcesChanged()
{
}
