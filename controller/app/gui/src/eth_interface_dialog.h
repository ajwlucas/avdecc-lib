#ifndef ETH_INTERFACE_DIALOG_H
#define ETH_INTERFACE_DIALOG_H

#include <QDialog>

namespace Ui {
class EthInterfaceDialog;
}

class EthInterfaceDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit EthInterfaceDialog(QWidget *parent = 0);
    ~EthInterfaceDialog();
    
private:
    Ui::EthInterfaceDialog *ui;
};

#endif // ETH_INTERFACE_DIALOG_H
