#ifndef ABSTRACTRECORD_H
#define ABSTRACTRECORD_H
#include <QtCore>
#include <QtSql>
class AbstractRecord{
public:
    AbstractRecord();
public:
    Q_INVOKABLE static QSqlRecord asRecord(const QSqlTableModel *model, QJsonObject json);
    Q_INVOKABLE static QJsonObject asJson(QSqlRecord record);
};

#endif // ABSTRACTRECORD_H
