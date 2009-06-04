#ifndef PROPERTYLISTENER_H
#define PROPERTYLISTENER_H

#include <QObject>

class ContextProperty;

class PropertyListener : public QObject
{
    Q_OBJECT
public:
    PropertyListener(ContextProperty *prop);
private slots:
    void onValueChanged();
private:
    ContextProperty *prop;
};

#endif
