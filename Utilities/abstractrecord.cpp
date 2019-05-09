#include "abstractrecord.h"

AbstractRecord::AbstractRecord()
{

}

QSqlRecord AbstractRecord::asRecord(const QSqlTableModel *model, QJsonObject json)
{
    //Gets Record from table without relation field names
    //Original table field
    auto record = model->database ().driver ()->record (model->tableName ());

    QStringList recordFields;
    for(int i = 0 ; i < record.count () ; ++i){
        recordFields << record.fieldName (i);
    }

    for(auto key : recordFields){
        auto value  = json[key].toVariant ();
        if(value.type () == QVariant::String){
            if(value.toString ().length () == 0)
                value = QJsonValue();
        }
        record.setValue (key,value);
    }

    return record;
}

QJsonObject AbstractRecord::asJson(QSqlRecord record)
{
    QStringList recordFields;
    for(int i = 0 ; i < record.count () ; ++i){
        recordFields << record.fieldName (i);
    }

    QJsonObject json;

    for(auto field : recordFields){
        json[field] = record.field (field).value ().toJsonValue ();
    }

    return json;
}
