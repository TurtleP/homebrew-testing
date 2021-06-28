#include <stdio.h>
#include <string.h>

#include <algorithm>
#include <list>
#include <vector>

#include <switch.h>

#include <joystick.h>

#include "log.h"

static constexpr int MAX_JOYSTICKS = 4;
static size_t g_activeCount        = 0;

static std::list<Joystick*> g_Joysticks;
static std::vector<Joystick*> g_ActiveJoysticks;

/*
** Get the active HidNpadId count
** @return size_t active players
*/
size_t getActiveControllerCount()
{
    size_t active = 0;
    for (size_t index = 0; index < MAX_JOYSTICKS; index++)
    {
        HidNpadIdType gamepadId = static_cast<HidNpadIdType>(HidNpadIdType_No1 + index);
        uint32_t styleSet       = hidGetNpadStyleSet(gamepadId);

        if (styleSet != 0)
            active++;
    }

    return active;
}

std::string getJoystickGUID(size_t index)
{
    if (index < 0 || index >= getActiveControllerCount())
        return std::string("");

    HidNpadIdType joystick = static_cast<HidNpadIdType>(HidNpadIdType_No1 + index);
    uint32_t styleSet      = hidGetNpadStyleSet(joystick);

    if (styleSet & HidNpadStyleTag_NpadFullKey)
        return guids[HidNpadStyleTag_NpadFullKey];
    else if (styleSet & HidNpadStyleTag_NpadHandheld)
        return guids[HidNpadStyleTag_NpadHandheld];
    else if (styleSet & HidNpadStyleTag_NpadJoyDual)
        return guids[HidNpadStyleTag_NpadJoyDual];
    else if (styleSet & HidNpadStyleTag_NpadJoyLeft)
        return guids[HidNpadStyleTag_NpadJoyLeft];
    else if (styleSet & HidNpadStyleTag_NpadJoyRight)
        return guids[HidNpadStyleTag_NpadJoyRight];

    return std::string("");
}

/*
** Get a Joystick by index
** @param size_t index to check
** @return Joystick pointer on success or nullptr on failure
*/
Joystick* getJoystickByIndex(size_t index)
{
    for (auto stick : g_ActiveJoysticks)
    {
        LOG("Stick #%zu / Index #%zu", stick->GetID(), index);
        if (stick->GetID() == index)
            return stick;
    }

    return nullptr;
}

Joystick* getJoystick(size_t index)
{
    if (index < 0 || index > g_ActiveJoysticks.size())
        return nullptr;

    return g_ActiveJoysticks[index];
}

/*
** Remove a joystick from the list of joysticks
** @param Joystick pointer to remove
*/
void removeJoystick(Joystick* joystick)
{
    if (!joystick)
        return;

    auto iterator = std::find(g_ActiveJoysticks.begin(), g_ActiveJoysticks.end(), joystick);

    if (iterator != g_ActiveJoysticks.end())
        g_ActiveJoysticks.erase(iterator);
}

/*
** Add a Joystick to the list of joysticks
** @return Joystick pointer
*/
Joystick* addJoystick(size_t index)
{
    if (index < 0 || index >= g_activeCount)
        return nullptr;

    std::string guid = getJoystickGUID(index);

    Joystick* joystick = nullptr;
    bool reused        = false;

    for (auto stick : g_Joysticks)
    {
        if (!stick->IsConnected() && stick->GetGUID() == guid)
        {
            joystick = stick;
            reused   = true;
            break;
        }
    }

    if (!joystick)
    {
        joystick = new Joystick(g_Joysticks.size());
        g_Joysticks.push_back(joystick);
    }

    removeJoystick(joystick);

    if (!joystick->Open(index))
        return nullptr;

    for (auto activeStick : g_ActiveJoysticks)
    {
        if (joystick->GetHandle() == activeStick->GetHandle())
        {
            joystick->Close();

            if (!reused)
                g_Joysticks.remove(joystick);

            return activeStick;
        }
    }

    g_ActiveJoysticks.push_back(joystick);

    return joystick;
}

/*
** Check for Joystick post-connected status
** @return Joystick pointer object that changed
*/
int checkJoystickAdded()
{
    size_t active = getActiveControllerCount();

    if (active > g_activeCount)
    {
        g_activeCount = active;
        return (active - 1);
    }

    return -1;
}

/*
** Check Joystick post-connection removal status
** @return Joystick pointer object that changed
*/
int checkJoystickRemoved()
{
    size_t active = getActiveControllerCount();

    /* check that the active count updated */
    if (active < g_activeCount)
    {
        g_activeCount = active;

        for (auto joystick : g_Joysticks)
            joystick->Update();

        return active;
    }

    return -1;
}

static constexpr const char* format = "[%s] Player %d : Active %d\n";

int main(int argc, char** argv)
{
    /* initialize console */
    consoleInit(NULL);

    /* Configure input layout */
    padConfigureInput(MAX_JOYSTICKS, HidNpadStyleSet_NpadStandard);

    g_activeCount = getActiveControllerCount();
    for (size_t index = 0; index < g_activeCount; index++)
        addJoystick(index);

    /* index player one */
    const auto playerOne = g_Joysticks.front();

    while (appletMainLoop())
    {
        consoleClear();

        printf("Checking for Controllers.. Plus to Quit\n");
        printf("Active %zu / Current %zu\n\n", getActiveControllerCount(), g_activeCount);

        printf("[Cached Joysticks]\n");
        for (auto joystick : g_Joysticks)
            printf(format, joystick->GetName().c_str(), joystick->GetID(), joystick->IsConnected());
        printf("\n");
        printf("[Active Joysticks]\n");
        for (auto joystick : g_ActiveJoysticks)
            printf(format, joystick->GetName().c_str(), joystick->GetID(), joystick->IsConnected());

        /* check statuses */
        int index = -1;
        if ((index = checkJoystickAdded()) != -1)
            addJoystick(index);

        if ((index = checkJoystickRemoved()) != -1)
            removeJoystick(getJoystickByIndex(index));

        /* update all the joysticks */
        for (auto joystick : g_ActiveJoysticks)
            joystick->Update();

        /* check our exit condition */
        if (playerOne->IsConnected())
        {
            if (playerOne->IsDown(HidNpadButton_Plus))
                break;
            else if (playerOne->IsDown(HidNpadButton_Left))
            {
                playerOne->Split();
                playerOne->Open(0);
            }
            else if (playerOne->IsDown(HidNpadButton_Right))
            {
                if (g_ActiveJoysticks.at(1))
                {
                    g_Joysticks.remove(g_ActiveJoysticks.at(1));
                    playerOne->Merge(g_ActiveJoysticks.at(1));
                    playerOne->Open(0);
                }
                else
                    printf("Failure to merge!\n");
            }
        }

        consoleUpdate(NULL);
    }

    for (auto stick : g_Joysticks)
    {
        delete stick;
        removeJoystick(stick);
    }

    consoleExit(NULL);

    return 0;
}
