
#include <QApplication>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QListView>
#include <QTableView>
#include <QWidget>

#include "alklinkmodel.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QVector<AlkLinkData> list = {
        {"Example 1", "ID001", "https://example1.com/{id}", "test1", "replace1", "uuid-001", QDate::currentDate()},
        {"Example 2", "ID002", "https://example2.com/{id}", "test2", "replace2", "uuid-002", QDate::currentDate()},
        {"Example 3", "ID003", "https://example3.com/{id}", "test3", "replace3", "uuid-003", QDate::currentDate()},
    };

    auto *model = new AlkLinkModel;
    model->setDataList(list);

    auto *listView = new QListView;
    model->setVerticalMode(false, 0); // show details for first item
    listView->setModel(model);
    listView->setModelColumn(0); // show "name"

    auto *detailsView = new QTableView;
    auto *model2 = new AlkLinkModel;
    detailsView->setModel(model2);
    model->setVerticalMode(true, 0); // show details for first item
    detailsView->horizontalHeader()->setStretchLastSection(true);

    QObject::connect(listView->selectionModel(), &QItemSelectionModel::currentChanged,
                     [model](const QModelIndex &current, const QModelIndex &) {
                         model->setVerticalMode(true, current.row());
                     });

    QWidget window;
    QHBoxLayout *layout = new QHBoxLayout(&window);
    layout->addWidget(listView);
    layout->addWidget(detailsView);
    window.resize(800, 400);
    window.show();

    return app.exec();
}
