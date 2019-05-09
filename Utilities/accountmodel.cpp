#include "accountmodel.h"

AccountModel::AccountModel(QObject *parent) : AbstractDataModel(parent)
{
    innerModel->setTable ("accnt");
    innerModel->select ();
}

