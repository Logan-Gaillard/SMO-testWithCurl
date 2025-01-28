#pragma once

#include "types.h"
#include "heap/seadHeapMgr.h"
#include "logger/Logger.hpp"

class DataStream {
    u8* mBuffer = nullptr;
    u32 mBufferSize = 0;
    u32 mBufferMaxSize = 0;
    u32 mBufferPos = 0;
    sead::Heap* mHeap = nullptr;

    bool resize(u32 amt) {
        Logger::log("-1\n");
        u32 newSize = mBufferMaxSize + (amt - (mBufferMaxSize - mBufferPos));

        if (mHeap->getFreeSize() > newSize) {
            Logger::log("-2\n");
            u8* newBuffer = (u8*)mHeap->tryRealloc(mBuffer, newSize, sizeof(u8*));
            if (!newBuffer){
                return false;
            }
            Logger::log("-3\n");
            mBuffer = newBuffer;
            Logger::log("-4\n");
            mBufferMaxSize = newSize;
            Logger::log("-5\n");
            return true;
        }
        return false;
    }

public:
    explicit DataStream(u32 startSize) {
        if (startSize > 0) {
            mBuffer = new u8[startSize]();
            mBufferMaxSize = startSize;
        }
        mBufferPos = 0;
        mHeap = sead::HeapMgr::instance()->getCurrentHeap();
    }

    ~DataStream() {
        Logger::log("Cleaning up Stream.\n");
        delete[] mBuffer;
    }

    template <typename T>
    u32 write(T* data, u32 len = sizeof(T)) {
        Logger::log("1\n");
        if (mBufferPos + len > mBufferMaxSize) {
            Logger::log("2\n");
            if (!resize(len)){
                Logger::log("3\n");
                return 0;
            }
        }
        Logger::log("4\n");
        memcpy(mBuffer + mBufferPos, data, len);
        Logger::log("5\n");
        mBufferPos += len;
        Logger::log("6\n");
        if (mBufferPos > mBufferSize){
            Logger::log("7\n");
            mBufferSize = mBufferPos;
        }
        return len;
    }

    template <typename T>
    u32 read(T* data, u32 len = sizeof(T)) {
        u32 readSize = mBufferPos + len > mBufferMaxSize ? len : mBufferMaxSize - mBufferPos;
        memcpy(data, mBuffer + mBufferPos, readSize);
        mBufferPos += readSize;
        return readSize;
    }

    void rewind(u32 len = 0) {
        if (len == 0)
            mBufferPos = 0;
        else {
            mBufferPos -= len;
            if (mBufferPos < 0)
                mBufferPos = 0;
        }
    }

    void skip(u32 len) {
        if (mBufferPos + len > mBufferMaxSize) {
            if (!resize(len))
                return;
        }
        memset(mBuffer + mBufferPos, 0, len);
        mBufferPos += len;
    }

    u32 getSize() const {
        return mBufferSize;
    }

    u32 getMaxSize() const {
        return mBufferMaxSize;
    }

    u8* getData() const {
        return mBuffer;
    }

    bool isAtEnd() const {
        return mBufferPos == mBufferMaxSize;
    }
};