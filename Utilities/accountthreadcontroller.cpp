#include "accountthreadcontroller.h"

AccountThreadController::AccountThreadController(QObject *parent) : AbstractThreadController(parent)
{

}

DatabaseToOperate AccountThreadController::database() const
{
  return ACCOUNT;
}
