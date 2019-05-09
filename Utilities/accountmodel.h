#ifndef ACCOUNTMODEL_H
#define ACCOUNTMODEL_H

#include "abstractdatamodel.h"

class AccountModel : public AbstractDataModel
{
    Q_OBJECT
public:
    explicit AccountModel(QObject *parent = nullptr);
};

#endif // ACCOUNTMODEL_H
