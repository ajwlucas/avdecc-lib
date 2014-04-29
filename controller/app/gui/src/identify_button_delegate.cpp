#include "identify_button_delegate.h"
#include <QEvent>
#include <QMouseEvent>
#include <QPainter>
#include "entity_table_model.h"
#include <QApplication>
#include <QStylePainter>
#include <QBrush>
#include <QPen>

IdentifyButtonDelegate::IdentifyButtonDelegate(QObject *parent) :
    QItemDelegate(parent)
{
    _state =  QStyle::State_Enabled | QStyle::State_Raised;
}

void IdentifyButtonDelegate::paint(QPainter *painter,
                   const QStyleOptionViewItem &option,
                   const QModelIndex &index) const
{
    QRect rect = option.rect;

    QRect buttonRect( rect);
    buttonRect.setX(rect.x()+ 7);
    buttonRect.setY(rect.y()+ 2);
    buttonRect.setHeight(25);
    buttonRect.setWidth(25);
    QStyleOptionButton button;
    button.rect = buttonRect;
    button.text = QChar(0xE70A);
    button.state = _state | QStyle::State_Enabled;
    button.features |= QStyleOptionButton::None;

    painter->setFont(QFont("Entypo", 32, QFont::Normal, false));

    QApplication::style()->drawControl(QStyle::CE_PushButton, &button, painter);
}

QSize IdentifyButtonDelegate::sizeHint(const QStyleOptionViewItem &/*option*/,
                       const QModelIndex &/*index*/) const
{
    //hard coding size for test purpose, 
    //actual size hint can be calculated from option param
    return QSize(30,30);
}

bool IdentifyButtonDelegate::editorEvent(QEvent *event, 
    QAbstractItemModel *model, 
    const QStyleOptionViewItem &option, 
    const QModelIndex &index)
{
    qDebug("event %d", event->type());
    if( event->type() == QEvent::MouseButtonPress ||
        event->type() == QEvent::MouseButtonRelease ) {
    } else {
         //ignoring other mouse event and reseting button's state
         // _state = QStyle::State_Raised;
        return true;
    }

    QRect buttonRect( option.rect);
    buttonRect.setHeight(25);
    buttonRect.setWidth(25);

    QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
    if( !buttonRect.contains( mouseEvent->pos()) ) {
        // _state = QStyle::State_Raised;
        qDebug("no");
        return true;
    }

    if( event->type() == QEvent::MouseButtonPress) {    
        qDebug("state: %d", _state);        
        if (_state & QStyle::State_Sunken) {
            _state |= QStyle::State_Raised;
            _state &= ~(QStyle::State_Sunken);
            qDebug("down");
        }
        else if (_state & QStyle::State_Raised) {
            _state |= QStyle::State_Sunken;
            _state &= ~(QStyle::State_Raised);
            qDebug("up");
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