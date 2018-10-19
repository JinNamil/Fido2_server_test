#include "HIDComm.h"
#include "Debug.h"
#include "stdafx.h"

#define FIDO_HID_PACKET_SIZE (64)

HIDComm::HIDComm(const char* in) : hidHandle(INVALID_HANDLE_VALUE)
{
    unsigned int inLen = (unsigned int)strlen(in);
    deviceName = new char[inLen+1]{0,};
	if ( deviceName != NULL )
		memcpy(deviceName, in, inLen);
}

HIDComm::~HIDComm()
{
    close();
    delete[] deviceName;
    deviceName = NULL;
}

int HIDComm::Read(void *buffer,  unsigned long length, unsigned int timeout)
{
    int ret = -1;
    unsigned long readBytes = 0;
    unsigned char readBuffer[FIDO_HID_PACKET_SIZE + 1] = {0,};

    open();
    readBytes = length + 1;
    ret = read(readBuffer, sizeof(readBuffer), &readBytes, timeout);
    close();
    if ( ret < 0 )
    {
        DBG_Log("read error");
        return -1;
    }

    memcpy(buffer, readBuffer + 1, readBytes - 1);

    return 0;
}

int HIDComm::Write(void *buffer, unsigned long length, unsigned int timeout)
{
    int ret = -1;
    unsigned long writeBytes = 0;
    unsigned char writeBuffer[FIDO_HID_PACKET_SIZE + 1] = {0,};

    if ( length > FIDO_HID_PACKET_SIZE )
    {
        writeBytes = FIDO_HID_PACKET_SIZE;
    }
    else
    {
        writeBytes = length;
    }

    memcpy(writeBuffer + 1, buffer, writeBytes);

    writeBytes++;
    open();
    ret = write(writeBuffer, sizeof(writeBuffer), &writeBytes, timeout);
    close();
    if ( ret < 0 )
    {
        DBG_Log("read error");
        return -1;
    }

    return 0;
}

int HIDComm::open(void)
{
    int ret = -1;

    if ( hidHandle != INVALID_HANDLE_VALUE )
    {
        ret = close();
        if ( ret < 0 )
        {
            DBG_Log("close error");
            return ret;
        }
    }

    if ( deviceName == NULL )
    {
        DBG_Log("deviceName is NULL");
        return -1;
    }

    hidHandle = CreateFile(deviceName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
    if ( hidHandle == INVALID_HANDLE_VALUE )
    {
        DBG_Log("hidHandle Open Error");
        return -1;
    }

    return 0;
}

int HIDComm::close(void)
{
    int ret = -1;

    if ( hidHandle == INVALID_HANDLE_VALUE )
    {
        return -1;
    }

//    ret = CancelIo(hidHandle);
//    if ( ret == 0 )
//    {
//        return -1;
//    }

    ret = CloseHandle(hidHandle);
    if ( ret == 0 )
    {
        return -1;
    }

    hidHandle = INVALID_HANDLE_VALUE;

    return 0;
}

int HIDComm::read(void *buffer,  unsigned long bufferLength, unsigned long  *receiveLength,  unsigned long timeout)
{
    int ret = -1;
    int err = -1;
    DWORD eventIndex;
    DWORD  readByte = 0;
    HANDLE abortEvent = INVALID_HANDLE_VALUE;
    HANDLE ioEvent  = INVALID_HANDLE_VALUE;
    HANDLE events[2] = { INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE};
    OVERLAPPED overlapped = { 0, };

    if ( buffer == NULL )
    {
        DBG_Log("buffer is NULL");
        return -1;
    }

    if ( receiveLength != NULL )
    {
        *receiveLength = 0;
    }

    abortEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if ( abortEvent == NULL )
    {
        err = GetLastError();
        DBG_Log("invalid abortEvent, 0x%08X", err);
        return -1;
    }
    else if ( GetLastError() == ERROR_ALREADY_EXISTS )
    {
        DBG_Log("ERROR_ALREADY_EXISTS");
        return -1;
    }

    ioEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if ( ioEvent == NULL )
    {
        err = GetLastError();
        DBG_Log("invalid ioEvent, 0x%08X", err);
        return -1;
    }
    else if ( GetLastError() == ERROR_ALREADY_EXISTS )
    {
        DBG_Log("ERROR_ALREADY_EXISTS");
        return -1;
    }

    events[0] = abortEvent;
    events[1] = ioEvent;
    memset(&overlapped, 0, sizeof(OVERLAPPED));
    overlapped.hEvent = ioEvent;

    BOOL running = TRUE;
    while ( running )
    {
        ret = ::ReadFile(hidHandle, buffer, bufferLength, receiveLength, &overlapped);
        if ( ret == FALSE )
        {
            err = GetLastError();
            if ( err != ERROR_IO_PENDING ) // failed
            {
                DBG_Log("error, 0x%08X", err);
                ret = FALSE;
                running = FALSE;
                continue;
            }
        }
        else
        {
            running = FALSE;
            continue;
        }

        /* ERROR_IO_PENDING */
        eventIndex = WaitForMultipleObjects(2, events, FALSE, timeout);
        switch ( eventIndex )
        {
            ResetEvent(events[eventIndex - WAIT_OBJECT_0]);
            case WAIT_OBJECT_0: // abordEvent: I/O Abort
                CancelIo(hidHandle);
                ret = FALSE;
                running = FALSE;
                continue;

            case WAIT_OBJECT_0 + 1: // ioEvent: I/O complete
                ret = TRUE;
                running = FALSE;
                ret = GetOverlappedResult(hidHandle, &overlapped, &readByte, TRUE);
                if (ret == FALSE)
                {
                    err = GetLastError();
                    continue;
                }
                break;

            default:
            case WAIT_FAILED:
                err = GetLastError();
            case WAIT_TIMEOUT:
                    DBG_Log("timeout");
                    running = FALSE;
                    ret = FALSE;
                continue;
        }
    }

    if ( readByte > 0 )
    {
        if ( receiveLength != NULL )
        {
            *receiveLength = readByte;
        }
    }

    if ( ioEvent != INVALID_HANDLE_VALUE )
    {
        CloseHandle(ioEvent);
        ioEvent = INVALID_HANDLE_VALUE;
    }

    if ( abortEvent != INVALID_HANDLE_VALUE )
    {
        CloseHandle(abortEvent);
        abortEvent = INVALID_HANDLE_VALUE;
    }

    if ( ret != (int)true )
    {
        DBG_Log("ret is not TRUE");
        return -1;
    }

    return 0;
}

int HIDComm::write(void *buffer, unsigned long bufferLength, unsigned long  *sendLength, unsigned long timeout)
{
    int ret = -1;
    int err = -1;
    DWORD eventIndex;
    DWORD  writeByte = 0;
    HANDLE abortEvent = INVALID_HANDLE_VALUE;
    HANDLE ioEvent  = INVALID_HANDLE_VALUE;
    HANDLE events[2] = { INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE};
    OVERLAPPED overlapped = { 0, };

    if ( buffer == NULL )
    {
        DBG_Log("buffer is NULL");
        return -1;
    }

    if ( sendLength != NULL )
    {
        *sendLength = 0;
    }

    abortEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if ( abortEvent == INVALID_HANDLE_VALUE )
    {
        err = GetLastError();
        DBG_Log("invalid abortEvent, 0x%08X", err);
        return -1;
    }
    else if ( GetLastError() == ERROR_ALREADY_EXISTS )
    {
        DBG_Log("ERROR_ALREADY_EXISTS");
        return -1;
    }

    ioEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if ( ioEvent == INVALID_HANDLE_VALUE )
    {
        err = GetLastError();
        DBG_Log("invalid ioEvent, 0x%08X", err);
        return -1;
    }
    else if ( GetLastError() == ERROR_ALREADY_EXISTS )
    {
        DBG_Log("ERROR_ALREADY_EXISTS");
        return -1;
    }

    events[0] = abortEvent;
    events[1] = ioEvent;
    memset(&overlapped, 0, sizeof(OVERLAPPED));
    overlapped.hEvent = ioEvent;

    BOOL running = TRUE;
    while ( running )
    {
        ret = ::WriteFile(hidHandle, buffer, bufferLength, sendLength, &overlapped);
        if ( ret == FALSE )
        {
            err = GetLastError();
            if ( err != ERROR_IO_PENDING ) // failed
            {
                DBG_Log("error, 0x%08X", err);
                ret = FALSE;
                running = FALSE;
                continue;
            }
        }
        else
        {
            running = FALSE;
            continue;
        }

        /* ERROR_IO_PENDING */
        eventIndex = WaitForMultipleObjects(2, events, FALSE, timeout);
        switch ( eventIndex )
        {
            ResetEvent(events[eventIndex - WAIT_OBJECT_0]);
            case WAIT_OBJECT_0: // abordEvent: I/O Abort
                CancelIo(hidHandle);
                ret = FALSE;
                running = FALSE;
                continue;

            case WAIT_OBJECT_0 + 1: // ioEvent: I/O complete
                ret = TRUE;
                running = FALSE;
                ret = GetOverlappedResult(hidHandle, &overlapped, &writeByte, TRUE);
                if (ret == FALSE)
                {
                    err = GetLastError();
                    continue;
                }
                break;

            default:
            case WAIT_FAILED:
                err = GetLastError();
            case WAIT_TIMEOUT:
                    DBG_Log("timeout");
                    running = FALSE;
                    ret = FALSE;
                continue;
        }
    }

    if ( writeByte > 0 )
    {
        if ( sendLength != NULL )
        {
            *sendLength = writeByte;
        }
    }

    if ( ioEvent != INVALID_HANDLE_VALUE )
    {
		if ( ioEvent != NULL )
			CloseHandle(ioEvent);
        ioEvent = INVALID_HANDLE_VALUE;
    }

    if ( abortEvent != INVALID_HANDLE_VALUE )
    {
		if ( abortEvent != NULL )
			CloseHandle(abortEvent);
        abortEvent = INVALID_HANDLE_VALUE;
    }

    if ( ret != (int)true )
    {
        DBG_Log("ret is not TRUE");
        return -1;
    }

    return 0;
}
