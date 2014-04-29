#ifndef ENTITY_HEADER_LABEL_H
#define ENTITY_HEADER_LABEL_H

#include <QMetaType>
#include <QString>

class EntityHeaderLabel
{
public:
    enum LabelType {
        ENTITY_NAME,
        STREAM_NAME
    };
    enum StreamType {
        TALKER,
        LISTENER
    };

    EntityHeaderLabel(EntityHeaderLabel::LabelType label_type,
                      EntityHeaderLabel::StreamType stream_type,
                      int entity_index,
                      int stream_index);
    EntityHeaderLabel(EntityHeaderLabel::LabelType label_type,
                      EntityHeaderLabel::StreamType stream_type,
                      int entity_index);
    bool isEntityName();
    bool isStreamName();
    bool isTalker();
    bool isListener();
    void get_label(QString &label);
private:
    QString label;
    bool is_entity_name;
    bool is_stream_name;
    bool talker;
    bool listener;
    int parent_entity_index;
    int stream_index;
};

// Q_DECLARE_METATYPE(EntityHeaderLabel)

#endif // ENTITY_HEADER_LABEL_H
