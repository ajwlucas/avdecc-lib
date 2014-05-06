#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QItemSelection>
#include <QTabWidget>
#include "eth_interface_dialog.h"
#include "entity_table_model.h"
#include "entity_router_model.h"
#include "entity_model.h"
#include "entity_sort_filter_proxy_model.h"
#include <QSortFilterProxyModel>

#include "net_interface.h"
#include "system.h"
#include "controller.h"
#include "util.h"
#include "enumeration.h"
#include "notification.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    int show_eth_interface_dialog();
    EntityTableModel *entity_table_model;
    EntityModel *entity_model;
    EntitySortFilterProxyModel *listener_proxy;
    EntitySortFilterProxyModel *talker_proxy;

    ~MainWindow();
    
    static void log_callback(void *user_obj, int32_t log_level, const char *log_msg, int32_t time_stamp_ms);
    static void wrapper_to_notification_callback(void *user_obj, int32_t notification_type, uint64_t guid, uint16_t cmd_type, uint16_t desc_type, uint16_t desc_index, uint32_t cmd_status, void *notification_id);

signals:
    void new_log(QString log_level, QString msg, int32_t time_stamp_ms);

private:
    Ui::MainWindow *ui;
    EthInterfaceDialog *eth_interface_dialog;

    avdecc_lib::net_interface *netif;
    avdecc_lib::system *sys;
    avdecc_lib::controller *controller_obj;
    avdecc_lib::util *utility;

    QWidget *rightTabs[3];
    QString rightTabLabels[3];
    QByteArray splitter_state;

    void insert_log_msg(QString log_level, QString msg, int32_t time_stamp_ms);
    void update_device_overview(const QItemSelection &selected, const QItemSelection &deselected);
    void send_identify(const QModelIndex &index, bool on);
    void left_tab_change(int index);

    // EntityTableModel *entity_table_model;

};

Q_DECLARE_METATYPE(uint32_t)

#endif // MAINWINDOW_H
