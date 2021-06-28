#pragma once

#include <string>
#include <unordered_map>

#include <switch.h>

/* this would go in a joystick module */

static std::unordered_map<HidNpadStyleTag, std::string> guids = {
    { HidNpadStyleTag_NpadHandheld, "{79D54EDE-A776-42EF-AD78-7564F22654C3}" },
    { HidNpadStyleTag_NpadFullKey, "{90971C95-54C7-4A40-B9B4-2F971946A699}" },
    { HidNpadStyleTag_NpadJoyDual, "{68BCBB18-5743-4E63-BEED-A18856BE0791}" },
    { HidNpadStyleTag_NpadJoyLeft, "{4514F958-E51B-4A98-8D8A-6B0CD8C183C5}" },
    { HidNpadStyleTag_NpadJoyRight, "{93C18D7D-32AC-4718-B0B8-65F50C5ED979}" }
};
