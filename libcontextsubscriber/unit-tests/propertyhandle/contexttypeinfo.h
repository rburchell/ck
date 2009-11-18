#ifndef CONTEXTTYPEINFO_H
#define CONTEXTTYPEINFO_H

#include <QVariant>

struct ContextTypeInfo
{
    static bool fail;
    bool typeCheck(const QVariant &value) const { return !fail; }
    QString name() const { return "PLASTIC"; }
};
#endif
