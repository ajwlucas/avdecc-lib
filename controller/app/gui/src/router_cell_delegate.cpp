#include "router_cell_delegate.h"
#include <QEvent>
#include <QMouseEvent>
#include <QPainter>
#include "entity_table_model.h"
#include <QApplication>
#include <QStylePainter>
#include <QBrush>
#include <QPen>
#include <QTableView>
#include <QDebug>

RouterCellDelegate::RouterCellDelegate(QObject *parent) :
    QItemDelegate(parent)
{
    _state =  QStyle::State_Enabled | QStyle::State_Raised;
}

void RouterCellDelegate::paint(QPainter *painter,
                   const QStyleOptionViewItem &option,
                   const QModelIndex &index) const
{
    QRect rect = option.rect;

    QRect buttonRect(rect);
    buttonRect.setX(rect.x()+1);
    buttonRect.setY(rect.y()+1);
    buttonRect.setHeight(18);
    buttonRect.setWidth(18);

    if (index.row() == index.column()) {
        // Outline:
        painter->setPen(Qt::NoPen);
        painter->setBrush(Qt::lightGray);
        painter->drawRect(buttonRect);
        return;
    }

    QStyleOptionButton button;
    button.rect = buttonRect;

    QVariant data = index.data();
    QString str = data.value<QString>();

    qDebug() << "data is" << str;
    // button.text = QChar(0xE70A);
    /*
    if (index.data()) {
        button.state = QStyle::State_Enabled | QStyle::State_Raised;
    }
    else {
        button.state = QStyle::State_Enabled | QStyle::State_Sunken;
    }
    */
    /*
    if (option.state & QStyle::State_Selected) {
        // painter->fillRect(option.rect, option.palette.highlight());
        qDebug("SELECTED");
    }
    */

    if (selected_index.isValid()) {
        qDebug("clicked %d %d, current: %d %d", index.row(), index.column(), selected_index.row(), selected_index.column());
    }

    if (selected_index.isValid() && (selected_index != index)) { // Clicked on a new entity
        button.state |= QStyle::State_Raised;
        button.state &= ~(QStyle::State_Sunken);
    }
    else if (!selected_index.isValid()) {
        button.state |= QStyle::State_Raised;
        button.state &= ~(QStyle::State_Sunken);        
    }
    else if (selected_index.isValid() && (selected_index == index)) {
        button.state = _state;
    }

    if (!selected_index.isValid() || (selected_index.isValid() && (index != selected_index))) {
        button.state |= QStyle::State_Raised;
        button.state &= ~(QStyle::State_Sunken);
        qDebug("index: %d %d", index.row(), index.column());
    } else {
        button.state = _state;
    }
    button.features |= QStyleOptionButton::None;

    painter->setFont(QFont("Entypo", 32, QFont::Normal, false));

    QApplication::style()->drawControl(QStyle::CE_PushButton, &button, painter);
}

QSize RouterCellDelegate::sizeHint(const QStyleOptionViewItem &/*option*/,
                       const QModelIndex &/*index*/) const
{
    //hard coding size for test purpose, 
    //actual size hint can be calculated from option param
    return QSize(18,18);
}

bool RouterCellDelegate::editorEvent(QEvent *event, 
    QAbstractItemModel *model, 
    const QStyleOptionViewItem &option, 
    const QModelIndex &index)
{
    if( event->type() == QEvent::MouseButtonPress ||
        event->type() == QEvent::MouseButtonRelease ) {
    } else {
        return true;
    }

    QTableView *parent_table = qobject_cast<QTableView *>(this->parent());
    RouterCellDelegate *item = qobject_cast<RouterCellDelegate *>(parent_table->itemDelegate(parent_table->indexAt(QPoint(1, 0))));

    QRect buttonRect( option.rect);
    buttonRect.setHeight(25);
    buttonRect.setWidth(25);

    QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
    if( !buttonRect.contains( mouseEvent->pos()) ) {
        return true;
    }

    if( event->type() == QEvent::MouseButtonPress) {       
        if (_state & QStyle::State_Sunken) {
            _state |= QStyle::State_Raised;
            _state &= ~(QStyle::State_Sunken);
            qDebug("up");
            selected_index = QModelIndex();
        }
        else if (_state & QStyle::State_Raised) {
            _state |= QStyle::State_Sunken;
            _state &= ~(QStyle::State_Raised);
            if (!selected_index.isValid()) {
                selected_index = index;
            }
            qDebug("down");
        }
    } else if( event->type() == QEvent::MouseButtonRelease) {
        // _state = QStyle::State_Raised;
        if (_state & QStyle::State_Raised) {
            emit buttonClicked(index, false);
        }
        else if (_state & QStyle::State_Sunken) {
            emit buttonClicked(index, true);
        }
    }    
    return true;
}