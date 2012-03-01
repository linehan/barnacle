#include "id.h"
#include "domain.h"
/*
  The itm_t is the fundamental encapsulation of the GeoJug engine.
  It contains three structures which track the vital information
  about an entity operating within the engine.
*/
struct itm_t {
        struct id_t *id;    // See "id.h"
        struct dom_t *dom;    // See "dom.h"
        void *data;
};


struct dom_t *  register_dom(unsigned char kind, TOC nibbles, GLOSS options);
uint32_t      register_itm(unsigned char kind, void *data);
struct itm_t *retreive_itm(unsigned char kind, uint32_t key);
