#pragma once
#ifndef __UI_DOCK_H
#define __UI_DOCK_H

void print_dock(void);
void dock_toggle(void);

void say_speak(wchar_t *icon, char *text);
void say_equip(wchar_t *icon, char *name);
void say_alert(wchar_t *text, short hicolor, short locolor);
void say_stats(uint32_t stat);

#endif
