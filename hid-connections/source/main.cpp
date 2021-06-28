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

/*
** Get a Joystick by index
** @param size_t index to check
** @return Joystick pointer on success or nullptr on failure
*/
Joystick* getJoystickByIndex(size_t index)
{
    for (auto stick : g_ActiveJoysticks)
    {
        if (stick->GetID() == index)
            return stick;
    }

    return nullptr;
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

    Joystick* joystick = nullptr;
    bool reused        = false;

    for (auto stick : g_Joysticks)
    {
        if (!stick->IsConnected())
        {
            joystick = stick;
            reused   = true;
            break;
        }
    }
    LOG("#%zu Reused? %d", index, reused);
    if (!joystick)
    {
        joystick = new Joystick(g_Joysticks.size());
        g_Joysticks.push_back(joystick);
    }

    removeJoystick(joystick);

    if (!joystick->Open(index))
        return nullptr;

    for (auto stick : g_ActiveJoysticks)
    {
        if (joystick == stick)
        {
            if (!reused)
                g_Joysticks.remove(joystick);

            return stick;
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
        return active;
    }

    return -1;
}

static constexpr const char* format = "[%s] Player %d : Active %d\x1b[K\n";

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

        printf("Checking for Controllers.. Plus to Quit\nActive %zu / Current %zu\n",
               getActiveControllerCount(), g_activeCount);

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
