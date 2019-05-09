#ifndef GREYOBJECT_H
#define GREYOBJECT_H

#include <QtCore>

class GreyObject : public QObject
{
    Q_OBJECT
public:
    explicit GreyObject(QObject *parent = nullptr);

signals:

public slots:
    QJsonArray properties();
    static QJsonArray propertiesFromObject(QObject *object);
    QVariant defaultValue(QJsonValue value);
    static QString dateString();
};

#endif // GREYOBJECT_H
