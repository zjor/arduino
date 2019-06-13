#ifndef _TIMER_H_
#define _TIMER_H_

typedef void (*timer_callback)(unsigned long);

class Timer {

  private:
    unsigned long interval = 0;
    unsigned long lastRunTimestamp = 0;
    timer_callback callback;

  public:
    Timer(unsigned long interval, timer_callback callback);    
    void run();

};

#endif
