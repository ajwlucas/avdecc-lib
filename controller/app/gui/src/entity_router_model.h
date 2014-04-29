#ifndef ENTITY_ROUTER_MODEL_H
#define ENTITY_ROUTER_MODEL_H

#include <QAbstractTableModel>
#include <QStandardItemModel>
#include "entity_header_label.h"
#include "controller.h"

class EntityRouterModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit EntityRouterModel(QObject *parent = 0);
    EntityRouterModel(avdecc_lib::controller *controller_ref);
    
    int rowCount(const QModelIndex &) const;
    int columnCount(const QModelIndex &) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex &index, int role) const;
signals:
    
public slots:

private:
    avdecc_lib::controller *controller_ref;
    // QStandardItemModel model;
    
};

#endif // ENTITY_ROUTER_MODEL_H
