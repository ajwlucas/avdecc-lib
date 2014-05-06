#include "entity_table_model.h"
#include <QTime>
#include <QStringList>
#include <QDebug>
#include <QFont>
#include <QCoreApplication>
#include <QSize>
#include "end_station.h"
#include "entity_descriptor.h"

EntityTableModel::EntityTableModel(QObject *parent) :
    QAbstractTableModel(parent)
{

}

EntityTableModel::EntityTableModel(const EntityTableModel &copy_from_me)
{
}

EntityTableModel::EntityTableModel(avdecc_lib::controller *controller_ref, EntityModel *entity_model)
{
    this->controller_ref = controller_ref;
    this->entity_model = entity_model;
}

int EntityTableModel::rowCount(const QModelIndex & index) const
{
   return entity_model->rowCount(index);
}

int EntityTableModel::columnCount(const QModelIndex & /*parent*/) const
{
    return 9;
}

void EntityTableModel::emitLayoutChanged()
{
    qDebug("EntityTableModel::emitLayoutChanged()");

    emit layoutChanged();
}

QVariant EntityTableModel::headerData(int section, Qt::Orientation orientation,
                                int role) const
{
    if (role == Qt::FontRole)
    {
        if (section == 0 || section == 6 || section == 7)
        {
            QFont iconFont = QFont("Entypo", 32, QFont::Normal, false);
            return iconFont;
        }
    }

    if (role == Qt::SizeHintRole) {
        int widths[9] = {40, 100, 100, 100, 100, 100, 40, 40, 40};
        if (section >= 0 && section < 9) {
            return QSize(widths[section], 20); 
        }
        else {
            return QVariant();
        }
    }

    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        uint c[2] = {QChar::surrogateToUcs4(0xD83D,0xDD12), 0};
        switch (section)
        {
        case 0: return QString::fromUcs4(c);
        case 1: return QString("Device Name");
        case 2: return QString("Manufacturer");
        case 3: return QString("Model");
        case 4: return QString("Firmware Version");
        case 5: return QString("Entity ID");
        case 6: return QString(QChar(0xE70A));
        case 7: return QString(QChar(0x2665));
        }
        return QVariant();
    }
    return QVariant();
}

int EntityTableModel::get_current_entity_and_descriptor(avdecc_lib::end_station *end_station,
        avdecc_lib::entity_descriptor **entity, avdecc_lib::configuration_descriptor **configuration) const
{
    *entity = NULL;
    *configuration = NULL;

    uint16_t current_entity = end_station->get_current_entity_index();
    if (current_entity >= end_station->entity_desc_count())
        return 1;

    *entity = end_station->get_entity_desc_by_index(current_entity);

    uint16_t current_config = end_station->get_current_config_index();
    if (current_config >= (*entity)->config_desc_count())
        return 1;

    *configuration = (*entity)->get_config_desc_by_index(current_config);

    return 0;
}

QVariant EntityTableModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int column = index.column();

    if (role == Qt::DisplayRole)
    {
        QVariant es = entity_model->data(index, EntityModel::EndStationPtrRole);
        auto end_station = dynamic_cast<avdecc_lib::end_station *>(es.value<avdecc_lib::end_station_imp *>());

        if (end_station)
        {
            uint64_t end_station_entity_id = end_station->entity_id();
            avdecc_lib::entity_descriptor *entity;
            avdecc_lib::configuration_descriptor *configuration;
            if(get_current_entity_and_descriptor(end_station, &entity, &configuration))
                return QVariant();

            const char *end_station_name = (const char *)entity->entity_name();
            const char *fw_ver = (const char *)entity->firmware_version();
            
            uint64_t end_station_mac = end_station->mac();

            const uint8_t *vendor = configuration->get_strings_desc_string_by_reference(entity->vendor_name_string());
            const uint8_t *model = configuration->get_strings_desc_string_by_reference(entity->model_name_string());

            QString s;

            switch (column)
            {
            case 1: return s.sprintf("%s", (entity ? end_station_name : "UNKNOWN"));
            case 2: return s.sprintf("%s", vendor);
            case 3: return s.sprintf("%s", model);
            case 4: return s.sprintf("%s", (entity ? fw_ver : "UNKNOWN"));
            case 5: return s.sprintf("%016llX", end_station_entity_id);
            }
        }
    }
    return QVariant();
}
