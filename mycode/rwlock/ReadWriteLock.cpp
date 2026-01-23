#include "ReadWriteLock.h"



void ReadWriteLock::readLock() {
    std::unique_lock<std::mutex> m_lock(_mutex);

    while (write_flag || write_wait_count > 0) {
        _cond.wait(m_lock);
    }
    read_count++;
}

void ReadWriteLock::readUnlock() {
    std::unique_lock<std::mutex> m_lock(_mutex);

    if (--read_count == 0) {
        m_lock.unlock();
        _cond.notify_all();
    }
}

void ReadWriteLock::writeLock() {
    std::unique_lock<std::mutex> m_lock(_mutex);

    ++write_wait_count;
    while (write_flag || read_count > 0) {
        _cond.wait(m_lock);
    }
    write_flag = true;
    --write_wait_count;
}

void ReadWriteLock::writeUnlock() {
    std::unique_lock<std::mutex> m_lock(_mutex);

    write_flag = false;
    _cond.notify_all();
}