#include "databasesingleton.h"
#include <QSqlDriver>

#include "sqlquerydatamodel.h"

std::unique_ptr<DatabaseSingleton> DatabaseSingleton::instance = 0;

DatabaseSingleton::DatabaseSingleton(QObject *parent) : QThread(parent)
{
    abort = false;
    restart = false;
}

DatabaseSingleton::~DatabaseSingleton()
{
    mutex.lock();
    abort = true;
    condition.wakeOne();
    mutex.unlock();
    wait();
}

void DatabaseSingleton::requestOperation(DatabaseToOperate db,OperationToPerform toPerfom, QVariant params, QList<Relation> relations)
{
    this->dbToOperate = db;
    this->toPerform = toPerfom;
    this->params = params;
    this->relations = relations;

    QueueItem queueItem;
    queueItem.dbToOperate = db;
    queueItem.toPerform = toPerfom;
    queueItem.params = params;

    mutex.lock ();
    if(!isRunning ()){
        queue << queueItem;
        start (LowPriority);

    }else if(isRunning () && queue.count () > 0){
        queue << queueItem;

    }else{

        queue << queueItem;
        restart = true;
        condition.wakeOne ();
    }
    mutex.unlock ();
}

void DatabaseSingleton::requestOperation(QueueItem queueItem)
{
    mutex.lock ();
    if(!isRunning ()){
        queue << queueItem;
        start (LowPriority);

    }else if(isRunning () && queue.count () > 0){
        queue << queueItem;

    }else{

        queue << queueItem;
        restart = true;
        condition.wakeOne ();
    }
    mutex.unlock ();
}



DatabaseSingleton *DatabaseSingleton::getInstance()
{
    if(!instance.get ()){
        instance = std::unique_ptr<DatabaseSingleton> (new DatabaseSingleton());
        instance->requestOperation (INITDB,INITOP);
    }

    return instance.get ();
}

void DatabaseSingleton::run()
{

//        //Temporary Database
        QSqlDatabase database = QSqlDatabase::addDatabase("QMYSQL","ThreadedDatabase");
        database.setDatabaseName ("erp_development");
        database.setHostName ("localhost");
        database.setPort (3306);
        database.setUserName ("root");
        database.setPassword ("12345");
        database.open ();


    //Temporary Database
//    QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE","ThreadedDatabase");
//    auto databaseName = QCoreApplication::applicationDirPath ().append ("/erp.db");
//    database.setDatabaseName (databaseName);
//    database.open ();

        forever{
            //Aborts returns thread if abort == true
            //Restart checks if to continue with operation
            if(abort){
                return;
            }

            mutex.lock ();
            auto queueItem = queue.takeLast ();
            auto db = queueItem.dbToOperate;
            auto operation = queueItem.toPerform;
            auto params = queueItem.params;
            auto relations = queueItem.relations;
            auto tableName = queueItem.tableName;
            auto controllerUuid = queueItem.controllerUuid;
            auto defaultSort = queueItem.defaultSort;
            auto defaultFilter = queueItem.defaultFilter;
            mutex.unlock ();

            std::unique_ptr<SqlQueryDataModel> dataModel(new SqlQueryDataModel);

            if(relations.count () > 0){
                for(auto relation : relations){
                    auto jsonObj = relation.toObject ();
                    auto row = jsonObj["row"].toString ();
                    auto tableName = jsonObj["tableName"].toString ();
                    auto indexColumn = jsonObj["indexColumn"].toString ();
                    auto displayColumn = jsonObj["displayColumn"].toString ();
                    dataModel->setRelation (jsonObj);
//                    dataModel->setRelation (row,tableName,indexColumn,displayColumn);
                }
            }

            dataModel->setDefaultSort (defaultSort);
            dataModel->setDefaultFilter (defaultFilter);
            dataModel->setTable (tableName);


            connect (dataModel.get(),&SqlQueryDataModel::jsonSignal,[=](QJsonArray json,QJsonObject lastRecord){
                emit this->jsonModel (json,controllerUuid,lastRecord);
            });

            connect (dataModel.get (),&SqlQueryDataModel::recordInserted,this,[=](QJsonObject record){
                emit this->recordInserted (controllerUuid,record);
            });
            connect (dataModel.get (),&SqlQueryDataModel::recordChanged,this,[=](QJsonObject record){
                emit this->recordChanged (controllerUuid,record);
            });
            connect (dataModel.get (),&SqlQueryDataModel::recordDeleted,this,[=](QJsonObject record){
                emit this->recordDeleted (controllerUuid,record);
            });


            //Condition db > ADDRESS prevents  from operating on null dataModel
            if(!tableName.isNull ()){

                //Emits to show that operations has started
                emit this->operationStarted ();

                switch (operation) {
                case INSERT : {
                    auto _param = params.toJsonObject ();
                    dataModel->insertRecord (_param);
                }
                    break;
                case SORT : {
                    auto _param = params.value<QPair<QString,Qt::SortOrder>>();
                    dataModel->sort (_param.first,_param.second);
                }
                    break;
                case SET :{
                    auto _param = params.toJsonObject ();
                    dataModel->setRecord (_param);
                }
                    break;
                case REMOVE : {
                    auto _param = params.toJsonObject ();
                    dataModel->removeRecord (_param);
                }
                    break;
                case FILTER : {
                    auto _param = params.toString ();
                    dataModel->filter (_param);
                }
                    break;
                case FETCH :{
                    //This sends the json from the operations
                    dataModel->modelAsJson ();
                }

                }
                //Sends signal that operations are stopped
                emit this->operationStopped ();
            }

            qDebug() << dataModel->lastError.text ();
            qDebug() << "Pausing Thread Now : " << QThread::currentThreadId ();

            //Sends thread to sleep to wait for next operation
            mutex.lock();
            if (!restart && queue.count () == 0)
                condition.wait(&mutex);
            restart = false;
            mutex.unlock();
        }

}
