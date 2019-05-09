#include "abstractthreadcontroller.h"
#include "databasesingleton.h"
int AbstractThreadController::controllerCounter = 0;

QString AbstractThreadController::getTableName() const
{
    return tableName;
}

void AbstractThreadController::setTableName(const QString &value)
{
    tableName = value;
}

AbstractThreadController::AbstractThreadController(QObject *parent) : QObject(parent)
{
    db = DatabaseSingleton::getInstance ();

    //creates controller UUid to identify each controller
    this->controllerUuid = (controllerCounter++);

    if(db != nullptr){

        connect (db,&DatabaseSingleton::recordInserted,this,[=](int controllerUUid, QJsonObject record){
            if(controllerUUid != this->controllerUuid) return ;
            emit recordInserted (record);

        });
        connect (db,&DatabaseSingleton::recordChanged,this,[=](int controllerUUid, QJsonObject record){
            if(controllerUUid != this->controllerUuid) return ;
            emit recordChanged (record);
        });
        connect (db,&DatabaseSingleton::recordDeleted,this,[=](int controllerUUid, QJsonObject record){
            if(controllerUUid != this->controllerUuid) return ;
            emit recordDeleted (record);

        });



        //Connects DatabaseSingleton to controller and checks if this controller calls
        //if this controller called database
        //emits signals with jsonArray & lastRecord
        connect (db,&DatabaseSingleton::jsonModel,[=](QJsonArray json, int controllerUuid, QJsonObject lastRecord){

            if(controllerUuid == this->controllerUuid){
                this->jsonArray = json;
                emit jsonChanged(jsonArray);

                if(!lastRecord.isEmpty ()){
                    this->lastRecord = lastRecord;
                    emit lastRecordChanged (lastRecord);
                }
            }
        });
    }
}

bool AbstractThreadController::insertRecord(const QJsonObject &record)
{
    db->requestOperation(getQueueItem (record,INSERT));
    return true;
}

bool AbstractThreadController::setRecord(const QJsonObject &record)
{
    db->requestOperation (getQueueItem (record, SET));
    return true;
}

bool AbstractThreadController::removeRecord(const QJsonObject &record)
{
    db->requestOperation (getQueueItem (record,REMOVE));
    return true;
}

void AbstractThreadController::sort(QString fieldName, Qt::SortOrder order)
{
    QPair<QString,Qt::SortOrder> _param;

    _param.first = fieldName;
    _param.second = order;

    auto params = QVariant::fromValue(_param);

    db->requestOperation (getQueueItem (params, SORT));
}

void AbstractThreadController::fetch()
{
    db->requestOperation (getQueueItem ("",FETCH,this->defaultSort,this->defaultFilter));
}

void AbstractThreadController::filter(QString value)
{
    db->requestOperation (getQueueItem (value,FILTER));
}

QJsonObject AbstractThreadController::createRelation(QString rowName, QString tableName, QString indexColumn, QString displayColumn, QString tableAlias)
{
    QJsonObject relation;
    relation["rowName"] = rowName;
    relation["tableName"] = tableName;
    relation["indexColumn"] = indexColumn;
    relation["displayColumn"] = displayColumn;
    relation["tableAlias"] = tableAlias;

    return relation;
}

void AbstractThreadController::clearRelations()
{
    relations = QJsonArray();
}

int AbstractThreadController::getControllerUuid() const
{
    return controllerUuid;
}

QJsonArray AbstractThreadController::getJsonArray() const
{
    return jsonArray;
}

QJsonObject AbstractThreadController::getLastRecord() const
{
    return lastRecord;
}

QueueItem AbstractThreadController::getQueueItem(QVariant params, OperationToPerform operation, QString defaultSort , QString defaultFilter)
{
    QueueItem queueItem;
    queueItem.controllerUuid = getControllerUuid ();
    queueItem.tableName = tableName;
    queueItem.toPerform = operation;
    queueItem.params = params;
    queueItem.relations = relations;
    queueItem.defaultSort = defaultSort;
    queueItem.defaultFilter = defaultFilter;
    return queueItem;
}
