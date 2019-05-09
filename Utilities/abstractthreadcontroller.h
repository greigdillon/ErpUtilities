#ifndef ABSTRACTTHREADCONTROLLER_H
#define ABSTRACTTHREADCONTROLLER_H

#include<QtCore>
#include<QtSql>
#include "relation.h"

class DatabaseSingleton;

enum DatabaseToOperate{
    INITDB = 0,
    NO_DB,
    ADDRESS,
    CONTACT,
    ACCOUNT,
};

enum OperationToPerform{
    INSERT,
    SET,
    REMOVE,
    SORT,
    FILTER,
    FETCH,
    INITOP,
    NO_OP
};

struct QueueItem
{
    QueueItem(){}
    DatabaseToOperate dbToOperate;
    OperationToPerform toPerform;
    QVariant params;
    QString tableName;
    int controllerUuid;
    QJsonArray relations;
    QString defaultSort;
    QString defaultFilter;
};

class AbstractThreadController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString tableName READ getTableName WRITE setTableName NOTIFY tableNameChanged)
    Q_PROPERTY(QJsonArray relations MEMBER relations  NOTIFY relationsChanged)
    Q_PROPERTY(QString sortFunction MEMBER sortFunction)
    Q_PROPERTY(QJsonArray json READ getJsonArray NOTIFY jsonChanged);
    Q_PROPERTY (QJsonObject lastRecord READ getLastRecord NOTIFY lastRecordChanged)
    Q_PROPERTY(QString defaultSort MEMBER defaultSort)
    Q_PROPERTY(QString defaultFilter MEMBER defaultFilter)

signals:
    void relationsChanged(QJsonArray relations );
    void sqlFunctionsChanged(QJsonArray sqlFunctions);

public:
    QJsonArray relations;
    QString sortFunction;

signals:
    void tableNameChanged(QString tableName);
    void jsonChanged(QJsonArray jsonArray);
    void lastRecordChanged(QJsonObject lastRecord);

public:
    QString tableName = 0;

public:
    explicit AbstractThreadController(QObject *parent = nullptr);
    ~AbstractThreadController(){
        qDebug() << "Deleting Abstract Controller : #"<< controllerUuid;
    }

    QString getTableName() const;
    void setTableName(const QString &value);

signals:
    void started();
    void stopped();

    void recordInserted(QJsonObject record);
    void recordChanged(QJsonObject record);
    void recordDeleted(QJsonObject record);
    void recordsFetched(QJsonArray json);

public slots:
    bool insertRecord(const QJsonObject &record);
    bool setRecord(const QJsonObject &record);
    bool removeRecord(const QJsonObject &record);
    void sort(QString fieldName, Qt::SortOrder order);
    void fetch();
    void filter(QString value);
    int getControllerUuid() const;
    QJsonArray getJsonArray() const;
    QJsonObject getLastRecord() const;
    QJsonObject createRelation(QString rowName, QString tableName, QString indexColumn, QString displayColumn, QString tableAlias = "");
    void clearRelations();


protected:
    DatabaseSingleton *db;
    QJsonArray jsonArray;


private:
    int controllerUuid;
    static int controllerCounter;
    QJsonObject lastRecord;
    QueueItem getQueueItem(QVariant params = QVariant(), OperationToPerform operation = NO_OP, QString defaultSort = "",QString defaultFilter = "");
    QString defaultSort;
    QString defaultFilter;
};

#endif // ABSTRACTTHREADCONTROLLER_H
