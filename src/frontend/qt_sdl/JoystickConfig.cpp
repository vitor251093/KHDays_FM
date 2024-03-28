/*
    Copyright 2016-2023 melonDS team

    This file is part of melonDS.

    melonDS is free software: you can redistribute it and/or modify it under
    the terms of the GNU General Public License as published by the Free
    Software Foundation, either version 3 of the License, or (at your option)
    any later version.

    melonDS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with melonDS. If not, see http://www.gnu.org/licenses/.
*/

#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "Platform.h"
#include "JoystickConfig.h"

namespace JoystickConfig
{
using namespace melonDS;

std::map<int, std::array<int, 12>> JoyMapping{};
std::map<int, std::array<int, HK_MAX>> HKJoyMapping{};

const char* kConfigFile = "joystick.ini";
const char* kUniqueConfigFile = "joystick.%d.ini";

JoystickConfigEntry Controls[] =
{
    {"Joy_A",      0,  -1},
    {"Joy_B",      1,  -1},
    {"Joy_Select", 2,  -1},
    {"Joy_Start",  3,  -1},
    {"Joy_Right",  4,  -1},
    {"Joy_Left",   5,  -1},
    {"Joy_Up",     6,  -1},
    {"Joy_Down",   7,  -1},
    {"Joy_R",      8,  -1},
    {"Joy_L",      9,  -1},
    {"Joy_X",      10, -1},
    {"Joy_Y",      11, -1},

    {"", -1, 0}
};

JoystickConfigEntry HKControls[] =
{
    {"HKJoy_Lid",                 HK_Lid,                 -1},
    {"HKJoy_Mic",                 HK_Mic,                 -1},
    {"HKJoy_Pause",               HK_Pause,               -1},
    {"HKJoy_Reset",               HK_Reset,               -1},
    {"HKJoy_FastForward",         HK_FastForward,         -1},
    {"HKJoy_FastForwardToggle",   HK_FastForwardToggle,   -1},
    {"HKJoy_FullscreenToggle",    HK_FullscreenToggle,    -1},
    {"HKJoy_SwapScreens",         HK_SwapScreens,         -1},
    {"HKJoy_SwapScreenEmphasis",  HK_SwapScreenEmphasis,  -1},
    {"HKJoy_SolarSensorDecrease", HK_SolarSensorDecrease, -1},
    {"HKJoy_SolarSensorIncrease", HK_SolarSensorIncrease, -1},
    {"HKJoy_FrameStep",           HK_FrameStep,           -1},
    {"HKJoy_PowerButton",         HK_PowerButton,         -1},
    {"HKJoy_VolumeUp",            HK_VolumeUp,            -1},
    {"HKJoy_VolumeDown",          HK_VolumeDown,          -1},

    {"", -1, 0}
};

std::map<int, std::array<int, 12>> JoyDefaultMapping = {};
std::map<int, std::array<int, HK_MAX>> HKJoyDefaultMapping = {};

void LoadFile(int inst)
{
    Platform::FileHandle* f;
    if (inst > 0)
    {
        char name[100] = {0};
        snprintf(name, 99, kUniqueConfigFile, inst+1);
        f = Platform::OpenLocalFile(name, Platform::FileMode::ReadText);
    }
    else
        f = Platform::OpenLocalFile(kConfigFile, Platform::FileMode::ReadText);

    if (!f) return;

    char linebuf[1024];
    char entryname[32];
    char entryval[1024];
    while (!Platform::IsEndOfFile(f))
    {
        if (!Platform::FileReadLine(linebuf, 1024, f))
            break;

        int ret = sscanf(linebuf, "%31[A-Za-z_0-9.]=%[^\t\r\n]", entryname, entryval);
        entryname[31] = '\0';
        if (ret < 2) continue;

        std::string entrynameStr = std::string(entryname);
        std::istringstream entrynameStream(entrynameStr);
        std::string joystickidStr;
        std::string buttonnameStr;
        std::getline(entrynameStream, joystickidStr, '.');
        std::getline(entrynameStream, buttonnameStr, '.');
        const char* buttonname = buttonnameStr.c_str();
        int joystickid = std::stoi(joystickidStr);

        if (JoyMapping.find(joystickid) == JoyMapping.end())
        {
            JoyMapping[joystickid] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
            HKJoyMapping[joystickid] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
        }

        for (JoystickConfigEntry* entry = &Controls[0]; entry->Value; entry++)
        {
            if (!strncmp(entry->Name, buttonname, 32))
            {
                if (inst > 0)
                    break;

                *(int*)JoyMapping[joystickid][entry->Index] = strtol(entryval, NULL, 10);
                break;
            }
        }

        for (JoystickConfigEntry* entry = &HKControls[0]; entry->Value; entry++)
        {
            if (!strncmp(entry->Name, entryname, 32))
            {
                if (inst > 0)
                    break;

                *(int*)HKJoyMapping[joystickid][entry->Index] = strtol(entryval, NULL, 10);
                break;
            }
        }
    }

    CloseFile(f);
}

void Load()
{
    JoyMapping.clear();
    HKJoyMapping.clear();

    for(std::map<int,std::array<int, 12>>::iterator it = JoyDefaultMapping.begin(); it != JoyDefaultMapping.end(); ++it)
    {
        for (JoystickConfigEntry* entry = &Controls[0]; entry->Value; entry++)
        {
            *(int*)JoyMapping[it->first][entry->Index] = it->second[entry->Index];
        }
    }
    for(std::map<int,std::array<int, HK_MAX>>::iterator it = HKJoyDefaultMapping.begin(); it != HKJoyDefaultMapping.end(); ++it)
    {
        for (JoystickConfigEntry* entry = &HKControls[0]; entry->Value; entry++)
        {
            *(int*)HKJoyMapping[it->first][entry->Index] = it->second[entry->Index];
        }
    }

    LoadFile(0);

    int inst = Platform::InstanceID();
    if (inst > 0)
        LoadFile(inst);
}

void Save()
{
    int inst = Platform::InstanceID();

    Platform::FileHandle* f;
    if (inst > 0)
    {
        char name[100] = {0};
        snprintf(name, 99, kUniqueConfigFile, inst+1);
        f = Platform::OpenLocalFile(name, Platform::FileMode::WriteText);
    }
    else
        f = Platform::OpenLocalFile(kConfigFile, Platform::FileMode::WriteText);

    if (!f) return;

    for(std::map<int,std::array<int, 12>>::iterator it = JoyMapping.begin(); it != JoyMapping.end(); ++it)
    {
        for (JoystickConfigEntry* entry = &Controls[0]; entry->Value; entry++)
        {
            if (inst > 0)
                continue;

            Platform::FileWriteFormatted(f, "%s.%d=%d\n", it->first, entry->Name, it->second[entry->Index]);
        }

        for (JoystickConfigEntry* entry = &HKControls[0]; entry->Value; entry++)
        {
            if (inst > 0)
                continue;

            if (HKJoyMapping.find(it->first) != HKJoyMapping.end()) {
                Platform::FileWriteFormatted(f, "%s.%d=%d\n", it->first, entry->Name, HKJoyMapping[it->first][entry->Index]);
            }
        }
    }

    CloseFile(f);
}

}
