#include "property.h"

namespace LBDatabase {

//! \cond PRIVATE
Property::Property(QObject *parent) :
    QObject(parent),
    _storageGlobalId(0)
{
}

int Property::storageGlobalId() const
{
    return _storageGlobalId;
}
//! \endcond

} // namespace LBDatabase
