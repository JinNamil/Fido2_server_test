#ifndef __HIDCOMM_H__
#define __HIDCOMM_H__

#include <windows.h>

class HIDComm
{
public:
        HIDComm(const char* deviceName);
        virtual ~HIDComm();
        int Read(void *buffer,  unsigned long readLength, unsigned int timeout);
        int Write(void *buffer, unsigned long sendLength, unsigned int timeout);
        struct Error { };

    protected:

    private:
        char* deviceName;
        HANDLE hidHandle;
        int open(void);
        int close(void);
        int read(void *buffer,  unsigned long bufferLength, unsigned long  *readLength,  unsigned long timeout);
        int write(void *buffer, unsigned long bufferLength, unsigned long  *sendLength,  unsigned long timeout);
};

#endif // __HIDCOMM_H__
