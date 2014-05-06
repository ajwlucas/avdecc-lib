#ifndef ENTITY_TABLE_MODEL_H
#define ENTITY_TABLE_MODEL_H

#include <QAbstractTableModel>
#include "controller.h"
#include "end_station.h"
#include "entity_descriptor.h"
#include "configuration_descriptor.h"
#include "entity_model.h"

class EntityTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit EntityTableModel(QObject *parent = 0);
    explicit EntityTableModel(const EntityTableModel& copy_from_me);
    EntityTableModel(avdecc_lib::controller *controller_ref, EntityModel *entity_model);
    
    int rowCount(const QModelIndex &) const;
    int columnCount(const QModelIndex &) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    void emitLayoutChanged();

signals:
    
public slots:

private:
    avdecc_lib::controller *controller_ref;
    EntityModel *entity_model;
    int get_current_entity_and_descriptor(avdecc_lib::end_station *end_station,
        avdecc_lib::entity_descriptor **entity, avdecc_lib::configuration_descriptor **configuration) const;
    
};

Q_DECLARE_METATYPE(EntityTableModel)
Q_DECLARE_METATYPE(QList<QPersistentModelIndex>)
Q_DECLARE_METATYPE(QAbstractItemModel::LayoutChangeHint)

#endif // ENTITY_TABLE_MODEL_H
