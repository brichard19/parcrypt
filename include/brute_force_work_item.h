#ifndef _BRUTE_FORCE_WORK_ITEM_H
#define _BRUTE_FORCE_WORK_ITEM_H

#include "work_unit.h"

namespace parcrypt {

class BruteForceWorkItem : public IWorkItem {

private:
  std::string _target;
  secp256k1::uint256 _start;
  secp256k1::uint256 _end;
  int _id = -1;

  std::string _private_key;

public:

  BruteForceWorkItem() {}
  BruteForceWorkItem(IWorkUnit* parent, int id, const std::string& target, secp256k1::uint256& start, secp256k1::uint256& end)
    :
    _id(id),
    _target(target),
    _start(start),
    _end(end)
  {
    _parent = parent;
  }

  virtual ~BruteForceWorkItem() {}

  int id()
  {
    return _id;
  }

  std::string target() const
  {
    return _target;
  }

  secp256k1::uint256 start() const
  {
    return _start;
  }

  secp256k1::uint256 end() const
  {
    return _end;
  }

  void set_private_key(const std::string& private_key)
  {
    _private_key = private_key;
  }

  std::string get_private_key()
  {
    return _private_key;
  }
};

}

#endif