#include "contactmodel.h"

ContactModel::ContactModel(QObject *parent) : AbstractDataModel(parent)
{
    //Implemenatation of AbstractDataModel is still needed for Relations

    innerModel->setTable ("cntct");
    innerModel->setRelation (6,QSqlRelation("accnt","uuid","accnt_name"));
    innerModel->select ();
}

