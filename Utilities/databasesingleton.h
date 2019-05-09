#ifndef DATABASESINGLETON_H
#define DATABASESINGLETON_H

#include "QSql"
#include "QtCore"
#include <QSqlDatabase>
#include <QSqlRelationalTableModel>
#include "QSqlRecord"
#include "Utilities/abstractthreadcontroller.h"
#include "relation.h"

class AddressThreadController;


Q_DECLARE_METATYPE (DatabaseToOperate);
Q_DECLARE_METATYPE (OperationToPerform);

class DatabaseSingleton : public QThread
{
    Q_OBJECT
public:
    ~DatabaseSingleton();

    Q_ENUM (DatabaseToOperate);
    Q_ENUM (OperationToPerform);

    friend AbstractThreadController;

signals:
    void addressJsonModel(QJsonArray json);
    void contactJsonModel(QJsonArray json);
    void accountJsonModel(QJsonArray json);
    void jsonModel(QJsonArray json, int controllerUuid, QJsonObject lastRecord = QJsonObject());
    void operationStarted();
    void operationStopped();

    void recordInserted(int controllerUuid, QJsonObject record);
    void recordChanged(int controllerUuid, QJsonObject record);
    void recordDeleted(int controllerUuid, QJsonObject record);
    void recordsFetched(int controllerUuid, QJsonArray json);

public slots:
    void requestOperation(DatabaseToOperate db, OperationToPerform toPerfom, QVariant params = "",QList<Relation> relations = QList<Relation>());
    void requestOperation(QueueItem queueItem);
    static DatabaseSingleton *getInstance();

private:
    explicit DatabaseSingleton(QObject *parent = nullptr);
    static std::unique_ptr<DatabaseSingleton> instance;
    QJsonObject _addressRecord;


    // QThread interface
protected:
    void run() override;
    QWaitCondition condition;
    QMutex mutex;
    DatabaseToOperate dbToOperate;
    OperationToPerform toPerform;
    QVariant params;
    bool restart;
    bool abort;
    QList<QueueItem> queue;
    QList<Relation>relations;
    QString tableName;
};

#endif // DATABASESINGLETON_H
