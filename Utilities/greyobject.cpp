#include "greyobject.h"

GreyObject::GreyObject(QObject *parent) : QObject(parent)
{

}

QJsonArray GreyObject::properties()
{
    return propertiesFromObject (this);
}

QJsonArray GreyObject::propertiesFromObject(QObject *object)
{
    QJsonArray array;
    auto metaObject = object->metaObject ();
    auto count = metaObject->propertyCount ();

    for(int i = 0 ; i < count ; ++i){
        auto index = i;
        auto propertyName = metaObject->property (index).name ();
        auto propertyString = QString(propertyName);
        auto _objectName = "objectName";

        if(!propertyString.contains (_objectName))
            array << propertyString;
    }
    return array;
}

QVariant GreyObject::defaultValue(QJsonValue value)
{
    auto typeId = value.type ();
    auto convertedType = QVariant().convert (typeId);
    return convertedType;
}

QString GreyObject::dateString()
{
    return  QDateTime::currentDateTime ().toString ();
}

