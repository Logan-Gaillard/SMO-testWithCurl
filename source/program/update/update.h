#pragma once

#include "sead/heap/seadDisposer.h"
#include "logger/Logger.hpp"
#include "cjson/cJSON.h"
//#include "UpdateApiInfo.h"
#include <devenv/seadDebugFontMgrNvn.h>
#include <gfx/seadTextWriter.h>

class Update {
    SEAD_SINGLETON_DISPOSER(Update);
    Update();
    ~Update();

public:
    void checkUpdate(bool isBeta, const char* tagVersion);
    void init(sead::Heap* heap) { 
        mHeap = heap;
    }
    bool downloadUpdate();
    void analyseJson(cJSON* json, bool isBeta);
    const char* getTag(){return mTag;};

private:
    bool downloadAndInstallContents(const char* url, const char* path);
    sead::Heap* mHeap;
    const char* mTag;
    //UpdateApiInfo* mInfo;
};