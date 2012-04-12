#ifndef __VERBS_H
#define __VERBS_H


struct verb_info {
        int     id;
        char    *name;
        char    *desc;
        wchar_t *icon;
        short   pair;
        void    (*say)(uint32_t from_key, uint32_t to_key);
};


#define MAXVERB 30
struct verb_info verbs[MAXVERB];


enum sayings { MSG_CANCEL, MSG_SMACK, SAY_INIT };


#endif



