#include <QItemDelegate>
class IdentifyButtonDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    IdentifyButtonDelegate(QObject *parent = 0);
    virtual void paint(QPainter *painter,
                       const QStyleOptionViewItem &option,
                       const QModelIndex &index) const ;

    virtual QSize sizeHint(const QStyleOptionViewItem &option,
                           const QModelIndex &index) const ;

    bool editorEvent(QEvent *event, QAbstractItemModel *model, 
                           const QStyleOptionViewItem &option, 
                           const QModelIndex &index);

signals:
    void buttonClicked(const QModelIndex &index, bool on);
private:
    QStyle::State  _state;
};