#ifndef _MONA_KOBJECT_
#define _MONA_KOBJECT_

#include <sys/types.h>

class Thread;

class KObject
{
public:
    KObject();
    virtual ~KObject();

public:
    virtual int checkSecurity(Thread* thread);
    virtual int getReferanceCount() const;
    virtual int getType() const;
    virtual void setReferance();
    virtual void cancelReferance();

    dword getId() const {return id;}
    void setId(dword id) {this->id = id;}

private:
    int referanceCount;

public:
    dword id;
    static const int KOBJECT;
    static const int THREAD;
    static const int KMUTEX;
    static const int KTIMER;
};

#endif