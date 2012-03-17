#pragma once
#ifndef __VERB_MODEL_H
#define __VERB_MODEL_H


#include "../lib/fifo.h"
struct verb_t {
        struct fifo_t give;  // verbs (verb_ids) you are sending
        struct fifo_t to;    // nouns (keys) you are sending verbs to
        struct fifo_t get;   // verbs (verb_ids) you are receiving
        struct fifo_t from;  // nouns (keys) who sent you these verbs 
        uint32_t mask;
        uint32_t bump;
        uint32_t fund;
        uint32_t send;
        uint32_t canc;
        uint32_t rec;
        bool forw;
        bool empty;
};


struct verb_info {
        wchar_t *wch;
        short pair;
        void (*say)(uint32_t subject_key, uint32_t object_key);
};
struct verb_info verbs[8];

uint32_t verb_new(uint32_t sender, uint32_t recipient, uint32_t verb);
uint32_t verb_send(uint32_t key);
uint32_t verb_do(uint32_t to);

#endif
