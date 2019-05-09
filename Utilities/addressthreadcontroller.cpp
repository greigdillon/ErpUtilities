#include "addressthreadcontroller.h"

AddressThreadController::AddressThreadController(QObject *parent) : AbstractThreadController(parent)
{

}

DatabaseToOperate AddressThreadController::database() const
{
  return ADDRESS;
}
