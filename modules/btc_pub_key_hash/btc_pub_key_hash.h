#ifndef _BTC_PUB_KEY_HASH_H
#define _BTC_PUB_KEY_HASH_H

#include "modules/btc_pub_key_hash/btc_pub_key_work_unit.h"


namespace parcrypt {
void btc_pubkey_worker(WorkerThreadState* state, parcrypt::IWorkItem* work_item_ptr);
}

#endif