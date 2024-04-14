#pragma once

#include "Mutexlock.h"
#include "noncopyable.h"
#include "FileUtil.h"
#include <string>
#include <memory>

class LogFile : noncopyable
{
public:
    LogFile(const std::string& basename,int flushEveryN = 1024);
    ~LogFile();

    void append(const char* logline,int len);
    void flush();
    bool rollFile();
    
private:
    void append_unlocked(const char* logline,int len);

    const std::string basename_;
    const int flushEveryN_;

    int count_;
    MutexLock mutex_;
    AppendFile* file_;
};