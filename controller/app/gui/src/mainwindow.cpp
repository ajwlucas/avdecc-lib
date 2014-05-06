#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QListWidget>
#include <QDebug>
#include <QAbstractItemView>
#include <QFontDatabase>
#include <QTime>
#include <QTabWidget>
#include <QSplitter>
#include <QTimer>
#include <math.h>
#include "identify_button_delegate.h"
#include "router_cell_delegate.h"
#include "router_header.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    if (!this->show_eth_interface_dialog()) {
        QString fontName(QLatin1String("/Users/andy/github/dc/avdecc-lib/controller/app/gui/resources/Entypo.ttf"));
        qDebug() << fontName;
        int appFontId = QFontDatabase::addApplicationFont(fontName);
        if (appFontId < 0){
            qWarning() << "Attempt to load application font: "<< fontName << " failed.";
        }
        QTimer::singleShot(500, this, SLOT(showMaximized()));
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    delete entity_table_model;
    delete eth_interface_dialog;
    delete entity_model;
    delete listener_proxy;
    sys->process_close();
    sys->destroy();
    controller_obj->destroy();
    netif->destroy();
    utility->destroy();
}

void MainWindow::log_callback(void *user_obj, int32_t log_level, const char *log_msg, int32_t time_stamp_ms)
{
    MainWindow* obj = (MainWindow*) user_obj;
    // qDebug("[LOG] %d (%s)\n", log_level, log_msg);
    QString levels[7] = {QString("ERROR"),
                         QString("WARNING"),
                         QString("NOTICE"),
                         QString("INFO"),
                         QString("DEBUG"),
                         QString("VERBOSE"),
                         QString("No Level")};
    /*
        LOGGING_LEVEL_ERROR   = 0,
        LOGGING_LEVEL_WARNING = 1,
        LOGGING_LEVEL_NOTICE  = 2,
        LOGGING_LEVEL_INFO    = 3,
        LOGGING_LEVEL_DEBUG   = 4,
        LOGGING_LEVEL_VERBOSE = 5,
        */
    if (log_level < 0 || log_level > 5) {
        log_level = 6;
    }

    emit obj->new_log(levels[log_level], QString(log_msg), time_stamp_ms);
}

void MainWindow::insert_log_msg(QString log_level, QString msg, int32_t time_stamp_ms)
{
    QTableWidget *log_table = this->centralWidget()->findChild<QTableWidget *>("eventLog");

    if (log_table) {
        log_table->insertRow(log_table->rowCount());
        QTime time = QTime::currentTime();
        QString timeString = time.toString();
        log_table->setItem(log_table->rowCount()-1, 0, new QTableWidgetItem(timeString));
        log_table->setItem(log_table->rowCount()-1, 1, new QTableWidgetItem(log_level));
        log_table->setItem(log_table->rowCount()-1, 2, new QTableWidgetItem(msg));
        if (log_level == "ERROR") {
            log_table->item(log_table->rowCount()-1, 0)->setBackground(Qt::red);
            log_table->item(log_table->rowCount()-1, 1)->setBackground(Qt::red);
            log_table->item(log_table->rowCount()-1, 2)->setBackground(Qt::red);
        }
    }
}

void MainWindow::wrapper_to_notification_callback(void *user_obj, int32_t notification_type, uint64_t guid, uint16_t cmd_type,
                                      uint16_t desc_type, uint16_t desc_index, uint32_t cmd_status, void *notification_id)
{
    MainWindow* mySelf = (MainWindow*) user_obj;

    if (notification_type == avdecc_lib::END_STATION_READ_COMPLETED ||
        notification_type == avdecc_lib::END_STATION_CONNECTED ||
        notification_type == avdecc_lib::END_STATION_DISCONNECTED)
    {
        qDebug("new entity");
        MainWindow* mySelf = (MainWindow*) user_obj;
        mySelf->entity_table_model->emitLayoutChanged();
        mySelf->entity_model->emitLayoutChanged();
    }
    // printf("%d, %d, %d\n", notification_type, cmd_type, desc_type);
    /*
    if (notification_type == avdecc_lib::RESPONSE_RECEIVED &&
        cmd_type == avdecc_lib::AEM_CMD_READ_DESCRIPTOR &&
        desc_type == avdecc_lib::AEM_DESC_ENTITY)
    {
        mySelf->entity_table_model->emitLayoutChanged();
    }
    */
}

void MainWindow::update_device_overview(const QItemSelection &selected, const QItemSelection &deselected)
{
    QLabel *l_entity_id = this->centralWidget()->findChild<QLabel *>("entity_id");
    QLabel *l_device_title = this->centralWidget()->findChild<QLabel *>("deviceTitle");
    QLabel *l_configuration = this->centralWidget()->findChild<QLabel *>("current_configuration");
    QLabel *l_talker_streams = this->centralWidget()->findChild<QLabel *>("talker_streams");
    QLabel *l_listener_streams = this->centralWidget()->findChild<QLabel *>("listener_streams");
    QLabel *l_group_name = this->centralWidget()->findChild<QLabel *>("group_name");
    QLabel *l_serial_number = this->centralWidget()->findChild<QLabel *>("serial_number");
    QLabel *l_firmware_version = this->centralWidget()->findChild<QLabel *>("firmware_version");

    QWidget *device_overview_widget = this->centralWidget()->findChild<QWidget *>("deviceOverviewWidget");

    if (selected.indexes().size()) {
        size_t row = selected.at(0).topLeft().row();
        qDebug() << row;
        auto end_station = controller_obj->get_end_station_by_index(row);

        if (end_station) {
            auto entity = end_station->get_entity_desc_by_index(end_station->get_current_entity_index());
            if (entity) {
                device_overview_widget->setEnabled(true);
                QString s;
                uint16_t current_config = end_station->get_current_config_index();
                auto configuration = entity->get_config_desc_by_index(current_config);
                if (configuration) {
                    const uint8_t *vendor = configuration->get_strings_desc_string_by_reference(entity->vendor_name_string());
                    if (vendor) {
                        l_device_title->setText(s.sprintf("%s %s", vendor, entity->entity_name()));
                    }
                    else {
                        l_device_title->setText("Unknown");
                    }
                    l_configuration->setText(s.sprintf("%s", configuration->object_name()));

                    QComboBox *interface_box = this->centralWidget()->findChild<QComboBox *>("interfaceComboBox");
                    interface_box->clear();

                    if (configuration->avb_interface_desc_count()) {
                        for (int i=0; i < configuration->avb_interface_desc_count(); i++) {
                            auto interface_desc = configuration->get_avb_interface_desc_by_index(i);
                            if (interface_desc) {
                                interface_box->addItem(s.sprintf("%s", interface_desc->object_name()));
                            }
                        }
                        int current_selected_index = interface_box->currentIndex();
                        auto interface_desc = configuration->get_avb_interface_desc_by_index(current_selected_index);

                        if (interface_desc) {
                            QLabel *l_mac_address = this->centralWidget()->findChild<QLabel *>("mac_address");
                            QLabel *l_gptp_clock_identity = this->centralWidget()->findChild<QLabel *>("gptp_clock_identity");
                            QLabel *l_gptp_domain_number = this->centralWidget()->findChild<QLabel *>("gptp_domain_number");
                            QLabel *l_gptp_port_number = this->centralWidget()->findChild<QLabel *>("gptp_port_number");

                            l_mac_address->setText(s.sprintf("%012llx", interface_desc->mac_addr()));
                            l_gptp_clock_identity->setText(s.sprintf("%016llx", interface_desc->clock_identity()));
                            l_gptp_domain_number->setText(s.sprintf("%d", interface_desc->domain_number()));
                            l_gptp_port_number->setText(s.sprintf("%d", interface_desc->port_number()));
                        }
                    }

                    auto audio_unit = configuration->get_audio_unit_desc_by_index(0);

                    if (audio_unit) {
                        QString sample_rates;
                        for (int i=0; i < audio_unit->sampling_rates_count(); i++) {
                            uint32_t rate = audio_unit->get_sampling_rate_by_index(i);
                            double rate_in_khz = (double)rate / (double)1000;
                            QString dummy;
                            if (rate_in_khz == floor(rate_in_khz)) { // check if whole number
                                sample_rates += dummy.sprintf("%.0f kHz ", rate_in_khz);
                            }
                            else {
                                sample_rates += dummy.sprintf("%.3f kHz", rate_in_khz);
                            }
                        }
                        QLabel *l_sample_rates = this->centralWidget()->findChild<QLabel *>("sample_rates");
                        l_sample_rates->setText(sample_rates);
                    }
                }
                else {
                    l_device_title->setText("Unknown");
                }
                l_entity_id->setText(s.sprintf("%016llx", end_station->entity_id()));
                l_firmware_version->setText(s.sprintf("%s", entity->firmware_version()));
                l_talker_streams->setText(s.sprintf("%d", entity->talker_stream_sources()));

                QFont iconFont = QFont("Entypo", 32, QFont::Normal, false);
                if (entity->talker_stream_sources()) {
                    QLabel *l_talker_capable = this->centralWidget()->findChild<QLabel *>("talker_capable");
                    l_talker_capable->setFont(iconFont);
                    l_talker_capable->setText(QString(QChar(0x2713))); // tick
                }

                l_listener_streams->setText(s.sprintf("%d", entity->listener_stream_sinks()));
                if (entity->listener_stream_sinks()) {
                    QLabel *l_listener_capable = this->centralWidget()->findChild<QLabel *>("listener_capable");
                    l_listener_capable->setFont(iconFont);
                    l_listener_capable->setText(QString(QChar(0x2713))); // tick
                }

                l_group_name->setText(s.sprintf("%s", entity->group_name()));
                l_serial_number->setText(s.sprintf("%s", entity->serial_number()));

                device_overview_widget->show();                    
            }
        }
    }
    else {
        device_overview_widget->setEnabled(false);
    }
}

void MainWindow::send_identify(const QModelIndex &index, bool on)
{
    avdecc_lib::end_station *end_station = controller_obj->get_end_station_by_index(index.row());

    intptr_t cmd_notification_id = 0;
    end_station->send_identify((void *)cmd_notification_id, on);
}

void MainWindow::left_tab_change(int index)
{
    QTabWidget *right_tab_widget = this->centralWidget()->findChild<QTabWidget *>("rightTabWidget");
    right_tab_widget->removeTab(0);
    QSplitter *splitter = this->centralWidget()->findChild<QSplitter *>("splitter");

    if (index < 3) {
        right_tab_widget->insertTab(0, rightTabs[index], rightTabLabels[index]);
        if (splitter_state.size()) splitter->restoreState(splitter_state);
    }
    else {
        splitter_state = splitter->saveState();
        // Collapse the left pane if it's the event log tab
        QList<int> currentSizes = splitter->sizes();
        currentSizes[1] = 0;
        splitter->setSizes(currentSizes);
    }
}

int MainWindow::show_eth_interface_dialog()
{
    utility = avdecc_lib::create_util();
    netif = avdecc_lib::create_net_interface();
    controller_obj = avdecc_lib::create_controller(netif, MainWindow::wrapper_to_notification_callback, this, MainWindow::log_callback, 5);
    sys = avdecc_lib::create_system(avdecc_lib::system::LAYER2_MULTITHREADED_CALLBACK, netif, controller_obj);

    int interface_num;

    QStringList ifs;

    for(uint32_t i = 0; i < netif->devs_count();i++)
    {
        char *dev_desc;
        dev_desc = netif->get_dev_desc_by_index(i);
        ifs << dev_desc;
    }

    eth_interface_dialog = new EthInterfaceDialog();
    QListWidget *list = eth_interface_dialog->findChild<QListWidget *>("listWidget");
    if (list)
    {
        list->addItems(ifs);
    }

    if (eth_interface_dialog->exec() == QDialog::Accepted)
    {
        netif->select_interface_by_num(list->currentRow()+1);
        sys->process_start();

        QSplitter *splitter = this->centralWidget()->findChild<QSplitter *>("splitter");
        QList<int> sizes { 500, 500 };
        splitter->setSizes(sizes);

        entity_model = new EntityModel(controller_obj);
        entity_table_model = new EntityTableModel(controller_obj, entity_model);

        QTabWidget *right_tab_widget = this->centralWidget()->findChild<QTabWidget *>("rightTabWidget");
        QWidget *tab_stream_router = right_tab_widget->findChild<QWidget *>("streamRouteView");

        QListWidget *listener_stream_list = this->centralWidget()->findChild<QListWidget *>("listenerStreamList");
        listener_proxy = new EntitySortFilterProxyModel(this, listener_stream_list, tab_stream_router);
        listener_proxy->setSourceModel(entity_model);
        listener_proxy->setDynamicSortFilter(true);
        listener_proxy->setFilterTalkers(true);
        QListView *listener_device_list = this->centralWidget()->findChild<QListView *>("listenerDeviceList");
        listener_device_list->setModel(listener_proxy);
        connect(listener_device_list->selectionModel(), &QItemSelectionModel::selectionChanged,
                listener_proxy, &EntitySortFilterProxyModel::handleDeviceSelectionChanged);
        connect(listener_stream_list->selectionModel(), &QItemSelectionModel::selectionChanged,
                listener_proxy, &EntitySortFilterProxyModel::handleStreamSelectionChanged);

        QListWidget *talker_stream_list = this->centralWidget()->findChild<QListWidget *>("talkerStreamList");
        talker_proxy = new EntitySortFilterProxyModel(this, talker_stream_list, tab_stream_router);
        talker_proxy->setSourceModel(entity_model);
        talker_proxy->setDynamicSortFilter(true);
        talker_proxy->setFilterListeners(true);
        QListView *talker_device_list = this->centralWidget()->findChild<QListView *>("talkerDeviceList");
        talker_device_list->setModel(talker_proxy);
        connect(talker_device_list->selectionModel(), &QItemSelectionModel::selectionChanged,
                talker_proxy, &EntitySortFilterProxyModel::handleDeviceSelectionChanged);
        connect(talker_stream_list->selectionModel(), &QItemSelectionModel::selectionChanged,
                talker_proxy, &EntitySortFilterProxyModel::handleStreamSelectionChanged);

        QTableView *device_table = this->centralWidget()->findChild<QTableView *>("tableView");
        device_table->setModel(entity_table_model);
        device_table->setSelectionBehavior(QAbstractItemView::SelectRows);
        device_table->resizeColumnsToContents();

        IdentifyButtonDelegate* identifyDelegate = new IdentifyButtonDelegate(device_table);
        device_table->setItemDelegateForColumn(4, identifyDelegate);

        QWidget *device_overview_widget = this->centralWidget()->findChild<QWidget *>("deviceOverviewWidget");
        device_overview_widget->setEnabled(false);

        QTabWidget *left_tab_widget = this->centralWidget()->findChild<QTabWidget *>("leftTabWidget");
        left_tab_widget->setCurrentIndex(0);

        QWidget *tab_end_station_prop = right_tab_widget->findChild<QWidget *>("endStationProperties");
        QWidget *tab_stream_properties = right_tab_widget->findChild<QWidget *>("streamPropertiesView");

        if (!tab_stream_router) qDebug("no streamRouteView");

        rightTabs[0] = tab_end_station_prop;
        rightTabs[1] = tab_stream_router;
        rightTabs[2] = tab_stream_properties;

        // rightTabLabels
        for (int i=0; i < 3; i++)
        {
            rightTabLabels[i] = right_tab_widget->tabText(i);
        }

        right_tab_widget->removeTab(1);
        right_tab_widget->removeTab(1);

        connect(left_tab_widget, &QTabWidget::currentChanged, this, &MainWindow::left_tab_change);
        connect(device_table->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::update_device_overview);
        connect(identifyDelegate, &IdentifyButtonDelegate::buttonClicked, this, &MainWindow::send_identify);
        connect(this, &MainWindow::new_log, this, &MainWindow::insert_log_msg);

        return 0;
    }
    else
    {
        qDebug("NO");
        return 1;
    }
}
