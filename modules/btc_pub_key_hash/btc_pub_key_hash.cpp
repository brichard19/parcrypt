#include "worker_thread.h"
#include "work_unit.h"
#include "util.h"
#include "platform.h"
#include "modules/btc_pub_key_hash/btc_pub_key_hash.h"

#include "modules/btc_pub_key_hash/btc_pub_key_hash_gpu.h"

void parcrypt::btc_pubkey_worker(WorkerThreadState* state, parcrypt::IWorkItem* work_item_ptr)
{
  parcrypt::BruteForceWorkItem* work_item = reinterpret_cast<parcrypt::BruteForceWorkItem*>(work_item_ptr);

  state->set_state(WorkerState::Initializing);
  BTCPubKeyHashGPU device(state->gpu);

  device.set_mem_usage(state->gpu_config.mem_usage);

  device.init(work_item->start(), work_item->end(), work_item->target());

  state->set_state(WorkerState::Running);

  device.start();
  while(!device.done()) {
    platform::sleep(1.0);

    state->set_progress(device.progress());
    state->set_rate(device.key_rate());

    // Program still running?
    if(!state->running()) {
      device.stop();
      break;
    }
  }

  if(device.done()) {
    work_item->set_status(parcrypt::IWorkItem::Completed);

    secp256k1::uint256 private_key;

    if(device.get_result(private_key)) {
      std::string private_key_str = private_key.to_string();
      work_item->set_private_key(private_key_str);
    }
  } else {
    work_item->set_status(parcrypt::IWorkItem::Aborted);
  }
}