#ifndef _BTC_PUBKEY_WORK_UNIT_H
#define _BTC_PUBKEY_WORK_UNIT_H

#include "work_unit.h"
#include "brute_force_work_unit.h"
#include "worker_thread.h"


namespace parcrypt {

class BTCPubKeyHashWorkUnit : public BruteForceWorkUnit {

public:

  BTCPubKeyHashWorkUnit() = default;

  virtual ~BTCPubKeyHashWorkUnit() {}

  std::string type()
  {
    return "BTC_PUB_KEY_HASH";
  }

  static IWorkUnit* factory()
  {
    return new BTCPubKeyHashWorkUnit();
  }
};

}

#endif