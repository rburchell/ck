#!/bin/bash
qdbusxml2cpp -c ManagerInterface \
    -p manageriface_p.h:manageriface.cpp \
    -i dbusvariantmap.h \
    -N Manager.xml

qdbusxml2cpp -c SubscriberInterface \
    -p subscriberiface_p.h:subscriberiface.cpp \
    -i dbusvariantmap.h \
    -N Subscriber.xml
