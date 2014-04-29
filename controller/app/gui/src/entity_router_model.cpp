#include <QFont>
#include "entity_router_model.h"
#include "entity_descriptor.h"
#include "end_station.h"

EntityRouterModel::EntityRouterModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

EntityRouterModel::EntityRouterModel(avdecc_lib::controller *controller_ref)
{
    this->controller_ref = controller_ref;
}

int EntityRouterModel::rowCount(const QModelIndex & /*parent*/) const
{
    int count = this->controller_ref->get_end_station_count();
    /*
    if (count > model.rowCount()) {
        QStandardItem *index = model.invisibleRootItem();
        index->setColumnCount(index->columnCount() + 1);
        index->setRowCount(index->rowCount() + 1);
        QStandardItem *item = new QStandardItem(false);
        // QModelIndex index = model.index(index->rowCount(), index->columnCount(), &model);
        // index->setData(index, false);
        model.setItem(index->rowCount(), index->columnCount(), item);
        qDebug("added row + column");
    }
    */
    return count;
}

int EntityRouterModel::columnCount(const QModelIndex & /*parent*/) const
{
    return this->controller_ref->get_end_station_count();
}

QVariant EntityRouterModel::headerData(int section, Qt::Orientation orientation,
                                int role) const
{
    if (role == Qt::DisplayRole)
    {
        avdecc_lib::end_station *end_station = controller_ref->get_end_station_by_index(section);
        if (end_station)
        {
            avdecc_lib::entity_descriptor *entity = end_station->get_entity_desc_by_index(0);

            if (entity)
            {
                const char *end_station_name = (const char *)entity->entity_name();
                return QString(end_station_name);
            }
        }

        return QVariant();

    }
    else if (role == Qt::TextAlignmentRole)
    {
        if (orientation == Qt::Horizontal) return Qt::AlignRight;
    }
    return QVariant();
}

QVariant EntityRouterModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int column = index.column();

    if (role == Qt::DisplayRole)
    {
        return QVariant();
    }
    return QVariant();
}
