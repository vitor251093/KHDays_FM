#ifndef PLUGIN_DEFAULT_H
#define PLUGIN_DEFAULT_H

#include "Plugin.h"
#include "NDS.h"

namespace Plugins
{
using namespace melonDS;

class PluginDefault : public Plugin
{
public:
    PluginDefault() {};

    bool isDebugEnabled = false;
    
    u32 applyCommandMenuInputMask(melonDS::NDS* nds, u32 InputMask, u32 CmdMenuInputMask, u32 PriorCmdMenuInputMask) {
        return InputMask;
    }
    void hudToggle(melonDS::NDS* nds) {
    }
    const char* getGameSceneName() {
        return "";
    }
    bool shouldSkipFrame(melonDS::NDS* nds) {
        return false;
    }
    bool refreshGameScene(melonDS::NDS* nds) {
        return false;
    }
    void setAspectRatio(melonDS::NDS* nds, float aspectRatio) {
    }
};
}

#endif
