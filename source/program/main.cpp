#include "lib.hpp"
#include "patches.hpp"
#include "nn/err.h"
#include "logger/Logger.hpp"
#include "fs.h"

#include <basis/seadRawPrint.h>
#include <prim/seadSafeString.h>
#include <resource/seadResourceMgr.h>
#include <filedevice/nin/seadNinSDFileDeviceNin.h>
#include <filedevice/seadFileDeviceMgr.h>
#include <filedevice/seadPath.h>
#include <resource/seadArchiveRes.h>
#include <framework/seadFramework.h>
#include <heap/seadHeapMgr.h>
#include <heap/seadExpHeap.h>
#include <devenv/seadDebugFontMgrNvn.h>
#include <gfx/seadTextWriter.h>
#include <gfx/seadViewport.h>

#include "al/util.hpp"
#include "game/StageScene/StageScene.h"
#include "game/System/GameSystem.h"
#include "game/System/Application.h"
#include "game/HakoniwaSequence/HakoniwaSequence.h"
#include "rs/util.hpp"
#include "al/fs/FileLoader.h"

#include "update/update.h"
#include "agl/utl.h"
#include "al/resource/Resource.h"

static const char *DBG_FONT_PATH   = "DebugData/Font/nvn_font_jis1.ntx";
static const char *DBG_SHADER_PATH = "DebugData/Font/nvn_font_shader_jis1.bin";
static const char *DBG_TBL_PATH    = "DebugData/Font/nvn_font_jis1_tbl.bin";

sead::TextWriter *gTextWriter;

bool newUpdateAvailable = false; //If new update is available
bool isDownloadingUpdate = false; //If update is downloading
bool isUpdateFailed = false; //If the updating failed
bool disableMusic = false;

void checkUpdate(){
    Update::instance()->checkUpdate(false, "V.0.1.0");
    if(strcmp(Update::instance()->getTag(), "V.0.1.0") != 0){
        newUpdateAvailable = true;
        Logger::log("New version available : %s\n", Update::instance()->getTag());
    }
}

void downloadUpdate(){
    if(newUpdateAvailable){
        isDownloadingUpdate = true;
        isUpdateFailed = Update::instance()->downloadUpdate();
    }
}


HOOK_DEFINE_TRAMPOLINE(GameSystemInit) {
    static void Callback(GameSystem *thisPtr) {

        sead::Heap* curHeap = sead::HeapMgr::instance()->getCurrentHeap();

        sead::DebugFontMgrJis1Nvn::createInstance(curHeap);

        if (al::isExistFile(DBG_SHADER_PATH) && al::isExistFile(DBG_FONT_PATH) && al::isExistFile(DBG_TBL_PATH)) {
            sead::DebugFontMgrJis1Nvn::sInstance->initialize(curHeap, DBG_SHADER_PATH, DBG_FONT_PATH, DBG_TBL_PATH, 0x100000);
        }

        sead::TextWriter::setDefaultFont(sead::DebugFontMgrJis1Nvn::sInstance);

        al::GameDrawInfo* drawInfo = Application::instance()->mDrawInfo;

        agl::DrawContext *context = drawInfo->mDrawContext;
        agl::RenderBuffer* renderBuffer = drawInfo->mFirstRenderBuffer;

        sead::Viewport* viewport = new sead::Viewport(*renderBuffer);

        gTextWriter = new sead::TextWriter(context, viewport);

        gTextWriter->setupGraphics(context);

        gTextWriter->mColor = sead::Color4f(1.f, 1.f, 1.f, 0.8f);

        sead::Heap* updaterHeap = sead::ExpHeap::create(5000000, "UpdateHeap", nullptr, 8, sead::Heap::HeapDirection::cHeapDirection_Forward, false);
        
        Update::createInstance(updaterHeap);
        Update::instance()->init(updaterHeap);
        
        Orig(thisPtr);

        checkUpdate();

    }
};

HOOK_DEFINE_TRAMPOLINE(ControlHook) {
    static void Callback(StageScene* scene) {
        Orig(scene);
        if(((al::isPadHoldZR(-1) && al::isPadTriggerZL(-1)) || (al::isPadTriggerZR(-1) && al::isPadHoldZL(-1))) && isDownloadingUpdate == false){
            downloadUpdate();
        }
    }
};


HOOK_DEFINE_TRAMPOLINE(DrawDebugMenu) {
    static void Callback(HakoniwaSequence *thisPtr) { 

        Orig(thisPtr);

        if(isDownloadingUpdate == true){
            disableMusic = true;
            
            al::GameDrawInfo* drawInfo = Application::instance()->mDrawInfo;
            agl::DrawContext *context = drawInfo->mDrawContext;
            
            sead::Color4f c((double)0, (double)0, (double)0, 1);

            sead::Vector3<float> p1l(-1, 1, 0); // top left
            sead::Vector3<float> p2l(1, 1, 0); // top right
            sead::Vector3<float> p3l(-1, -1, 0); // bottom left
            sead::Vector3<float> p4l(1, -1, 0); // bottom right

            agl::utl::DevTools::beginDrawImm(context, sead::Matrix34<float>::ident, sead::Matrix44<float>::ident);

            agl::utl::DevTools::drawTriangleImm(context, p1l, p2l, p3l, c);
            agl::utl::DevTools::drawTriangleImm(context, p3l, p4l, p2l, c);

            gTextWriter->beginDraw();
            gTextWriter->setCursorFromTopLeft(sead::Vector2f(490, 340));
            gTextWriter->setScaleFromFontHeight(20);
            if(isUpdateFailed == false){
                gTextWriter->printf("Success, please reboot the game\n");
            }else{
                gTextWriter->printf("Update failed, please reboot the game\n");
            }
            gTextWriter->endDraw();
        }else{
            gTextWriter->beginDraw();
            gTextWriter->setCursorFromTopLeft(sead::Vector2f(10.f, 10.f));
            gTextWriter->setScaleFromFontHeight(20.f);
            if(newUpdateAvailable){
                gTextWriter->printf("A new update is available !");
            }
            gTextWriter->endDraw();
        }
    }
};

extern "C" void exl_main(void* x0, void* x1) {
    /* Setup hooking enviroment. */
    //envSetOwnProcessHandle(exl::util::proc_handle::Get());
    exl::hook::Initialize();

    if(LOGGER_IP != "0.0.0.0")
        Logger::instance().init(LOGGER_IP, 3080);

    runCodePatches();

    GameSystemInit::InstallAtOffset(0x535850); //Very important part

    // Debug Text Writer Drawing
    DrawDebugMenu::InstallAtOffset(0x50F1D8);

    ControlHook::InstallAtSymbol("_ZN10StageScene7controlEv");

}

extern "C" NORETURN void exl_exception_entry() {
    /* TODO: exception handling */
    EXL_ABORT(0x420);
}