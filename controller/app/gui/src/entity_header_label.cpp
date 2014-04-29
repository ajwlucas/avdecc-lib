#include "entity_header_label.h"
#include <assert.h>

EntityHeaderLabel::EntityHeaderLabel(EntityHeaderLabel::LabelType label_type,
                  EntityHeaderLabel::StreamType stream_type,
                  int entity_index,
                  int stream_index)
{

    if (label_type == LabelType::ENTITY_NAME) EntityHeaderLabel::is_entity_name = true;
    else EntityHeaderLabel::is_stream_name = true;

    if (stream_type == StreamType::TALKER) this->talker = true;
    else this->listener = true;
}

EntityHeaderLabel::EntityHeaderLabel(LabelType label_type, StreamType stream_type, int entity_index)
{
    assert(label_type == LabelType::ENTITY_NAME);
    EntityHeaderLabel::EntityHeaderLabel(label_type,
                      stream_type,
                      entity_index,
                      0);
}

bool EntityHeaderLabel::isEntityName()
{
    return is_entity_name;
}

bool EntityHeaderLabel::isStreamName()
{
    return is_stream_name;
}


bool EntityHeaderLabel::isTalker()
{
    return talker;
}

bool EntityHeaderLabel::isListener()
{
    return listener;
}

void EntityHeaderLabel::get_label(QString &label)
{
    label = this->label;
}
