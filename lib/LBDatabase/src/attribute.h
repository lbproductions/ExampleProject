#ifndef LBDATABASE_ATTRIBUTE_H
#define LBDATABASE_ATTRIBUTE_H

#include "property.h"

namespace LBDatabase {

class Attribute : public Property
{
    Q_OBJECT
public:

    enum Type {
        Unkown,     // 0
        Text,       // 1
        Integer,    // 2
        Real,       // 3
        Icon,       // 4
        Pixmap,     // 5
        DateTime,   // 6
        Time,       // 7
        Bool,       // 8
        Color,      // 9
        Enum        //10
    };
    Attribute(QObject *parent) : Property(parent) {}
    ~Attribute() {}

    virtual int columnIndex() const = 0;

    virtual bool isCalculated() const = 0;
    virtual bool cacheData() const = 0;
    virtual bool isEditable() const = 0;

    virtual Type type() const = 0;
    virtual QString typeName() const = 0;
    static QString typeToName(Type type);
    static QStringList typeNames();

    virtual QString qtType() const = 0;
    static QStringList qtTypeNames();
    static QString typeToQtType(Type type);
};

} // namespace LBDatabase

#endif // LBDATABASE_ATTRIBUTE_H
