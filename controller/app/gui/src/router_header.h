#ifndef ROUTER_HEADER_H
#define ROUTER_HEADER_H

#include <QHeaderView>
#include <QPainter>
#include <QMouseEvent>

class RouterHeader : public QHeaderView
{
    Q_OBJECT
public:
    explicit RouterHeader(Qt::Orientation orientation, QWidget *parent = 0);
    
    QSize sizeHint() const;
signals:
    
public slots:

protected:
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const;
    
};

#endif // ROUTER_HEADER_H
