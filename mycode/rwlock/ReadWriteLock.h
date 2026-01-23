#ifndef _READ_WRITE_LOCK_H
#define _READ_WRITE_LOCK_H

#include <mutex>
#include <condition_variable>

class ReadWriteLock {

public:
    void readLock();
    void readUnlock();
    void writeLock();
    void writeUnlock();

private:
    int read_count = 0;
    bool write_flag = false;
    int write_wait_count = 0; //写优先的关键变量，没有这个变量是读优先

    std::mutex _mutex;
    std::condition_variable _cond;
};


#endif //_READ_WRITE_LOCK_H