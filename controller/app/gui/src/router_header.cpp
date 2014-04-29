#include "router_header.h"

RouterHeader::RouterHeader(Qt::Orientation orientation, QWidget *parent) :
    QHeaderView(orientation, parent)
{
}

QSize RouterHeader::sizeHint() const
{
    // Get the base implementation size.
    QSize baseSize = QHeaderView::sizeHint();

    // Override the height with a custom value.
    baseSize.setHeight(150);

    return baseSize;
}

void RouterHeader::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    if (!rect.isValid())
        return;

    // get the state of the section
    QStyleOptionHeader opt;
    initStyleOption(&opt);
    QStyle::State state = QStyle::State_None;
    if (isEnabled())
        state |= QStyle::State_Enabled;
    if (window()->isActiveWindow())
        state |= QStyle::State_Active;

    /*
    if (this->sectionsClickable()) {
        if (logicalIndex == this->hover())
            state |= QStyle::State_MouseOver;
        if (logicalIndex == this->pressed)
            state |= QStyle::State_Sunken;
        else if (this->highlightSelected) {
            if (this->sectionIntersectsSelection(logicalIndex))
                state |= QStyle::State_On;
            if (this->isSectionSelected(logicalIndex))
                state |= QStyle::State_Sunken;
        }

    }
    */
    if (isSortIndicatorShown() && sortIndicatorSection() == logicalIndex)
        opt.sortIndicator = (sortIndicatorOrder() == Qt::AscendingOrder)
                            ? QStyleOptionHeader::SortDown : QStyleOptionHeader::SortUp;

    // setup the style options structure
    QVariant textAlignment = this->model()->headerData(logicalIndex, this->orientation(),
                                                  Qt::TextAlignmentRole);
    opt.rect = rect;
    opt.section = logicalIndex;
    opt.state |= state;
    opt.textAlignment = Qt::Alignment(textAlignment.isValid()
                                      ? Qt::Alignment(textAlignment.toInt())
                                      : this->defaultAlignment());

    opt.iconAlignment = Qt::AlignVCenter;
    opt.text = this->model()->headerData(logicalIndex, this->orientation(),
                                    Qt::DisplayRole).toString();
    if (this->textElideMode() != Qt::ElideNone)
        opt.text = opt.fontMetrics.elidedText(opt.text, this->textElideMode() , rect.width() - 4);

    QVariant variant = this->model()->headerData(logicalIndex, this->orientation(),
                                    Qt::DecorationRole);
    opt.icon = qvariant_cast<QIcon>(variant);
    if (opt.icon.isNull())
        opt.icon = qvariant_cast<QPixmap>(variant);
    QVariant foregroundBrush = this->model()->headerData(logicalIndex, this->orientation(),
                                                    Qt::ForegroundRole);
    if (foregroundBrush.canConvert<QBrush>())
        opt.palette.setBrush(QPalette::ButtonText, qvariant_cast<QBrush>(foregroundBrush));

    QPointF oldBO = painter->brushOrigin();

    QVariant backgroundBrush = this->model()->headerData(logicalIndex, this->orientation(),
                                                    Qt::BackgroundRole);
    if (backgroundBrush.canConvert<QBrush>()) {
        opt.palette.setBrush(QPalette::Button, qvariant_cast<QBrush>(backgroundBrush));
        opt.palette.setBrush(QPalette::Window, qvariant_cast<QBrush>(backgroundBrush));
        painter->setBrushOrigin(opt.rect.topLeft());
    }

    // the section position
    int visual = visualIndex(logicalIndex);
    Q_ASSERT(visual != -1);
    if (count() == 1)
        opt.position = QStyleOptionHeader::OnlyOneSection;
    else if (visual == 0)
        opt.position = QStyleOptionHeader::Beginning;
    else if (visual == count() - 1)
        opt.position = QStyleOptionHeader::End;
    else
        opt.position = QStyleOptionHeader::Middle;
    opt.orientation = this->orientation();
    // the selected position
    /*
    bool previousSelected = this->isSectionSelected(this->logicalIndex(visual - 1));
    bool nextSelected =  this->isSectionSelected(this->logicalIndex(visual + 1));
    if (previousSelected && nextSelected)
        opt.selectedPosition = QStyleOptionHeader::NextAndPreviousAreSelected;
    else if (previousSelected)
        opt.selectedPosition = QStyleOptionHeader::PreviousIsSelected;
    else if (nextSelected)
        opt.selectedPosition = QStyleOptionHeader::NextIsSelected;
    else
    */
        opt.selectedPosition = QStyleOptionHeader::NotAdjacent;

    int x_origin = 0;
    int y_origin = 0;
    int width = rect.width();
    int height = rect.height();

    if (this->orientation() == Qt::Vertical) {
            x_origin = 12;
            // width -= 12;
    }
    else {
            y_origin = 12;
            // height -= 12;
    }

    QRect new_r(x_origin, y_origin,  width, height);
    opt.rect = new_r;
    QPixmap original_button(rect.width(), rect.height());
    original_button.fill(Qt::transparent);
    QPainter original_button_painter(&original_button);
    style()->drawControl(QStyle::CE_HeaderSection, &opt, &original_button_painter, this);

    if (this->orientation() == Qt::Horizontal)
    {
        new_r.setY(0);
        new_r.setX(-5);
        new_r.setWidth(rect.height());
        new_r.setHeight(rect.width());
        QMatrix m;
        m.translate(0, rect.height()+y_origin);
        m.rotate(-90);
        original_button_painter.setWorldMatrix(m, true);
    }
    else
    {
        new_r.setX(x_origin+5);
        new_r.setY(0);
        new_r.setWidth(rect.width());
        new_r.setHeight(rect.height());

    }
    opt.rect = new_r;
    style()->drawControl(QStyle::CE_HeaderLabel, &opt, &original_button_painter, this);

    painter->drawPixmap(rect, original_button);

    /*
    if (logicalIndex == 0) {
        QPixmap pixmap( "/Users/andy/icons/expand_collapse_plus.gif" );
        painter->drawPixmap( QPoint( rect.left(), rect.top()+2 ), pixmap );
    }
    */

    painter->setBrushOrigin(oldBO);
}
