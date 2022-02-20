#ifndef _WORKER_DEVICE_H
#define _WORKER_DEVICE_H

class WorkerDevice {

public:
  virtual void start() = 0;
  
  virtual void stop() = 0;

  virtual double key_rate() = 0;

  virtual float progress() = 0;

  virtual bool done() = 0;

  virtual ~WorkerDevice() {}
};

#endif