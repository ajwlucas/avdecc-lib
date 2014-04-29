#include "eth_interface_dialog.h"
#include "ui_eth_interface_dialog.h"

EthInterfaceDialog::EthInterfaceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EthInterfaceDialog)
{
    ui->setupUi(this);
}

EthInterfaceDialog::~EthInterfaceDialog()
{
    delete ui;
}
