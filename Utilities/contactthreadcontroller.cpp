#include "contactthreadcontroller.h"

ContactThreadController::ContactThreadController(QObject *parent) : AbstractThreadController(parent)
{

}

DatabaseToOperate ContactThreadController::database() const
{
    return CONTACT;
}
