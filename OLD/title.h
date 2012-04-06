
WINDOW *wwin;
PANEL *ppan;
cchar_t bgtile;

        //༕ᚠᚡᛈ
        if (ppan == NULL) {
                wwin = newwin(LINES, COLS, 0, 0);
                ppan = new_panel(wwin);
                setcchar(&bgtile, L"ᚡ", 0, TITLE_SCREEN, NULL);
                wbkgrnd(wwin, &bgtile);
                scr_refresh();
        }
        else {
                top_panel(ppan);
                scr_refresh();
        }
