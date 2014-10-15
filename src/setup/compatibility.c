//
// Copyright(C) 2005-2014 Simon Howard
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//

// Sound control menu

#include <stdlib.h>

#include "m_config.h"
#include "textscreen.h"
#include "mode.h"

#include "compatibility.h"

// [cndoom] turn off limits by default
int vanilla_savegame_limit = 0;
int vanilla_demo_limit = 0;

void CompatibilitySettings(void)
{
    txt_window_t *window;

    window = TXT_NewWindow("Compatibility");

    TXT_AddWidgets(window, 
                   TXT_NewCheckBox("Vanilla savegame limit",
                                   &vanilla_savegame_limit),
                   TXT_NewCheckBox("Vanilla demo limit",
                                   &vanilla_demo_limit),
                   NULL);
}

void BindCompatibilityVariables(void)
{
    if (gamemission == doom || gamemission == strife)
    {
        M_BindVariable("vanilla_savegame_limit", &vanilla_savegame_limit);
        M_BindVariable("vanilla_demo_limit",     &vanilla_demo_limit);
    }
}

