#pragma once

#include "al/LiveActor/LiveActorGroup.h"
namespace al
{
    class LiveActorKit {
        public:
            int mMaxActors;
            void* mActorResourceHolder;
            void* mAreaObjDirector;
            void* mExecuteDirector;
            void* mGravityHolder;
            void* mEffectSystem;
            void* mGraphicsSystemInfo;
            void* gap_0;
            void* mModelDrawBufferUpdater;
            void* mExecuteAsyncExecutor1; // View Update (Core 1)
            void* mExecuteAsyncExecutor2; // View Update (Core 2)
            void* mModelDisplayListController;
            void* mExecuteDirector2;
            void* mLiveActorGroup;
            void* mCameraDirector;
            void* mClippingDirector;
            void* mCollisionDirector;
            void* gap_1;
            void* mPlayerHolder;
            void* mHitSensorDirector;
            void* mScreenPointDirector;
            void* mShadowDirector;
            void* mStageSwitchDirector;
            void* mSwitchAreaDirector;
            LiveActorGroup* mLiveActorGroup2;
            void* gap_2;
            void* mGamePadSystem;
            void* mPadRumbleDirector;
            void* mNatureDirector;
            void* mModelGroup;
    };
} // namespace al
