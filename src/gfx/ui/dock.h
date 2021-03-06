#pragma once
#ifndef __UI_DOCK_H
#define __UI_DOCK_H

void print_dock(void);
void dock_toggle(void);
void show_dock(void);
void hide_dock(void);

void say_speak(const wchar_t *icon, const char *text);
void say_equip(wchar_t *icon, char *name);
void say_alert(wchar_t *text, short hicolor, short locolor);
void say_stats(uint32_t stat);

void print_paused(void);

#endif
