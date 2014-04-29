#include "mainwindow.h"
#include <stdint.h>
#include <QApplication>
#include <QStyleFactory>
#include "entity_table_model.h"

QPalette setup_palette()
{
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(0x1D,0x12,0x29)); //1D1229
    darkPalette.setColor(QPalette::WindowText, QColor(0xE4, 0xE4, 0xE4)); // wrhite
    darkPalette.setColor(QPalette::Base, QColor(53,53,53));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53,53,53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, QColor(0xE4, 0xE4, 0xE4));
    darkPalette.setColor(QPalette::Button, QColor(53,53,53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218)); // 310062
 
    darkPalette.setColor(QPalette::Highlight, QColor(0x1D, 0x12, 0x29));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);

    return darkPalette;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<EntityTableModel>();
    qRegisterMetaType<QList<QPersistentModelIndex>>();
    qRegisterMetaType<QAbstractItemModel::LayoutChangeHint>();
    qRegisterMetaType<int32_t>("int32_t");
    MainWindow w;

    a.setStyle(QStyleFactory::create("Fusion"));
    
    /*
    QPalette darkPalette = setup_palette();
    a.setPalette(darkPalette);
    a.setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");
    */

    // w.show();

    return a.exec();
}
