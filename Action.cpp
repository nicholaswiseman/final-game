#include "Action.h"

Action::Action()
{
    m_name = eAction::ActionCount;
    m_type = eActionType::ActionTypeCount;
}

Action::Action(eAction name, eActionType type)
    : m_name(name)
    , m_type(type)
{

}

const eAction Action::GetName() const
{
    return m_name;
}

const eActionType Action::GetType() const
{
    return m_type;
}
