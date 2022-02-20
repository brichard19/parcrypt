#include "work_unit_manager.h"

#include "modules/btc_pub_key_hash/btc_pub_key_hash.h"

namespace parcrypt {

const std::map<std::string, WorkUnitFactory>& work_unit_factories()
{
  static const std::map<std::string, WorkUnitFactory> factories = {
    {"BTC_PUB_KEY_HASH", BTCPubKeyHashWorkUnit::factory}
  };

  return factories;
}

const std::map<std::string, WorkerFactory>& worker_factories()
{
  static const std::map<std::string, WorkerFactory> factories = {
    {"BTC_PUB_KEY_HASH", parcrypt::btc_pubkey_worker}
  };

  return factories;
}

} // parcrypt