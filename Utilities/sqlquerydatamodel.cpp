#include "sqlquerydatamodel.h"

SqlQueryDataModel::SqlQueryDataModel(QObject *parent) : QObject(parent)
{
    db = QSqlDatabase::database ("ThreadedDatabase");
    readAllQuery = QSqlQuery(db);
}

QString SqlQueryDataModel::createUuid()
{
    return QUuid::createUuid ().toString ().remove ("-").remove ("{").remove ("}");
}

void SqlQueryDataModel::modelAsJson()
{

    auto filterString = getDefaultFilter ().length () > 0 ? QString(" WHERE %1 ").arg (defaultFilter) : "";
    auto relationWithFilter = selectAllWithRelations ().append (filterString);
    auto sortString = QString("%1 ORDER BY %2").arg (relationWithFilter).arg(getDefaultSort ());
    QString toSend = getDefaultSort ().length () > 0 ? sortString : relationWithFilter;

    QSqlQuery allRelationQuery(toSend,db);
    allRelationQuery.exec ();
    modelAsJson (allRelationQuery);
}

void SqlQueryDataModel::modelAsJson(QSqlQuery &query)
{
    QJsonArray array;
    while(query.next ()){
        auto currentRecord = query.record ();
        array << AbstractRecord::asJson (currentRecord);
    }
    emit jsonSignal (array,lastRecord);
}

QJsonObject SqlQueryDataModel::record()
{
    return lastRecord;
}

bool SqlQueryDataModel::insertRecord(const QJsonObject &record)
{
    auto generatedRecord = setSystemParameters (record);
    auto fields = orignalFieldsNoPrepend.join (",");
    QStringList valueBindList;
    for(auto field : orignalFieldsNoPrepend){
        valueBindList << "?";
    }

    auto bindString = valueBindList.join (", ");
    auto insertString = QString("INSERT INTO %1 (%2) VALUES(%3)").arg(tableName).arg (fields).arg (bindString);

    QSqlQuery insertQuery(db);
    insertQuery.prepare (insertString);

    auto count = orignalFieldsNoPrepend.count ();

    for(int i = 0 ; i < count ; ++i){
        auto field = orignalFieldsNoPrepend.at (i);
        insertQuery.bindValue (i,generatedRecord[field].toVariant ());
    }

    insertQuery.exec ();
    modelAsJson ();

    setLastRecord (generatedRecord);
    recordInserted (lastRecord);

    this->lastError = insertQuery.lastError ();
    return true;
}

bool SqlQueryDataModel::setRecord(const QJsonObject &record)
{
    auto uuid = record["uuid"].toString ();
    if(uuid.isEmpty ()) return false;
    QStringList setList;
    for(auto field : orignalFieldsNoPrepend){
            setList <<QString( "%1 = ?").arg (field);
    }
    auto setListString = setList.join (",");
    auto setStatement = QString("UPDATE %1 SET %2 WHERE uuid='%3'").arg (tableName).arg (setListString).arg (uuid);

    QSqlQuery updateQuery(db);
    updateQuery.prepare (setStatement);
    auto count = orignalFieldsNoPrepend.count ();

    for(int i = 0 ; i < count ; ++i){
        auto value  = record[orignalFieldsNoPrepend.at (i)];
        updateQuery.bindValue (i,value.toVariant ());
    }

    updateQuery.exec ();
    modelAsJson ();

    setLastRecord (record);
    recordChanged (lastRecord);
    this->lastError = updateQuery.lastError ();

    return true;
}

bool SqlQueryDataModel::removeRecord(const QJsonObject &record)
{
    QSqlQuery deleteQuery(db);
    auto uuid = record["uuid"].toString ();

    if(uuid.isEmpty ()) return false;

    auto deleteString = QString("DELETE FROM %1 WHERE uuid='%2'").arg (tableName).arg (uuid);
    deleteQuery.exec (deleteString);
    modelAsJson ();

    this->lastRecord = record;
    recordDeleted (record);
    return true;
}

void SqlQueryDataModel::sort(QString fieldName, Qt::SortOrder order)
{
    auto sortOrder = order == Qt::AscendingOrder ? "ASC" : "DESC";
    auto sortString = QString("%3 ORDER BY %1 %2").arg (fieldName).arg (sortOrder).arg (selectAllWithRelations ());
    QSqlQuery sortQuery(db);
    sortQuery.exec (sortString);
    modelAsJson (sortQuery);

}

void SqlQueryDataModel::filter(QString value)
{
    if(value.length () == 0){
         modelAsJson ();
         return;
    }
    QSqlQuery filterQuery(db);
    filterQuery.exec (getFilterString (value));
    modelAsJson (filterQuery);
}

void SqlQueryDataModel::setRelation(const QString row, const QString &tableName, const QString &indexColumn, const QString &displayColumn)
{
    relations << Relation(row,tableName,indexColumn,displayColumn);
}

void SqlQueryDataModel::setRelation(QJsonObject relation)
{
    Relation r(relation);
    relations << r;
}

void SqlQueryDataModel::setTable(QString tableName)
{
    this->tableName = tableName;
    readAllQuery.exec (selectAll ());
    setFields();
    readAllQuery.exec (selectAllWithRelations ().append (" LIMIT 1"));
    tableRecord = readAllQuery.record ();
}

QString SqlQueryDataModel::getFilterString(QString value, bool isLastRecord)
{
    QStringList filterList;
    selectAllWithRelations ();

    int count = tableRecord.count();

    if(isLastRecord){
        filterList << QString("uuid ='%1' ").arg (value);
    }else{
        for(int i = 0; i < count ; ++i){
            auto field = tableRecord.fieldName (i);
            //Added this line to avoid filtering uuid but this is needed for last record
            if(field == "uuid") continue;

            auto filterField =QString( "%1 LIKE '%%2%' ").arg (field).arg (value.trimmed ());
            filterList << filterField;
        }
    }

    auto allFilters = filterList.join (" OR ");
    auto filterParams = QString("WHERE %1").arg(allFilters);
    auto filterString = QString("SELECT * FROM(%1) base ").arg(selectAllWithRelations ()).append (filterParams);

    return filterString;
}

void SqlQueryDataModel::setLastRecord(QJsonObject record){
    QSqlQuery _lastRecord(db);
    auto _filterString = getFilterString (record["uuid"].toString (),true);
    _lastRecord.exec (_filterString);
    _lastRecord.first ();

    this->lastRecord = AbstractRecord::asJson (_lastRecord.record ());
}

QString SqlQueryDataModel::selectAll()
{
    return  QString("SELECT * FROM %1 LIMIT 1").arg (tableName);
}

QString SqlQueryDataModel::selectAllWithRelations()
{
    auto joinType = "LEFT JOIN";
    QStringList relationList;
    QStringList relationFields;

    for(auto relation : relations){
        QString alias = relation.tableAlias.isEmpty () ? "" : "as "+relation.tableAlias;

        auto joinString = QString(" %1 %2 %6 ON %3.%4 = %5 ")
                .arg (joinType)
                .arg(relation.tableName)
                .arg(this->tableName)
                .arg(relation.rowName)
                .arg (relation.indexColumn)
                .arg (alias);

        auto emptyTable = relation.tableName.isEmpty ();
        if(!emptyTable)
            relationList << joinString;

        auto relationFieldWithTable = !emptyTable ? QString(" %1.%2 ")
                .arg (relation.tableAlias.isEmpty () ? relation.tableName : relation.tableAlias)
                .arg (relation.displayColumn) : QString("%1").arg (relation.displayColumn);

        relationFields << relationFieldWithTable;
        fieldsMapWithTable[relationFieldWithTable] = fieldsMapWithTable.count ();
    }

    relationList.removeDuplicates ();
    auto relationString = relationList.join ("");
    auto relationFieldString = relationFields.join (",");
    QStringList allFields;
    allFields << relationFields << originalFields;

    auto relationOriginalSelect = allFields.join (",");
    auto relationalString = QString("SELECT %1 FROM %2 %3")
            .arg (relationOriginalSelect)
            .arg(tableName)
            .arg (relationString);
    return relationalString;
}

void SqlQueryDataModel::setFields()
{
    readAllQuery.first ();
    auto record = readAllQuery.record ();
    auto fieldCount = record.count ();

    for(int i = 0 ; i < fieldCount ; ++i){
        auto fieldName = record.fieldName (i);
        orignalFieldsNoPrepend << fieldName;
        auto tableField = fieldName.prepend (QString("%1.").arg (tableName));
        originalFields << tableField;
        fieldsMapWithTable[tableField] = fieldsMapWithTable.count ();
    }
}

QJsonObject SqlQueryDataModel::setSystemParameters(QJsonObject record)
{
    QJsonObject obj = record;
    obj["uuid"] = createUuid();
    obj[tableName+"_created_date"] = QDateTime::currentDateTime ().toString ();
    return obj;
    //To add dates created by, modified date

}

QString SqlQueryDataModel::getDefaultFilter() const
{
    return defaultFilter;
}

void SqlQueryDataModel::setDefaultFilter(const QString &value)
{
    defaultFilter = value;
}

QString SqlQueryDataModel::getDefaultSort() const
{
    return defaultSort;
}

void SqlQueryDataModel::setDefaultSort(const QString &value)
{
    defaultSort = value;
}
