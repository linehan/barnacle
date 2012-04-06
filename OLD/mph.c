#include <assert.h>
#include <cmph.h>
#include <string.h>
#include "gfx/gfx.h"
#include "pan/test.h"
#include "mph.h"


inline char *mydup(const char *s)
{
        size_t len = 1+strlen(s);
        char *p = malloc(len);

        return p ? memcpy(p, s, len) : NULL;
}


struct mph_t *new_mph(const char *my_filename, unsigned int nkeys)
{
        assert(my_filename != NULL);

        struct mph_t *new;
        new = malloc(sizeof(struct mph_t));

        new->fn      = mydup(my_filename);
        new->keyring = calloc(nkeys, sizeof(char *));
        new->z       = nkeys;
        new->n       = 0;

        return (new);
}


unsigned int mph_add(struct mph_t *tab, const char *key)
{
        assert(tab != NULL);
        assert(key != NULL);

        if (tab->n > tab->z) {
                size_t newsize = ((2*tab->z) * sizeof(tab->keyring));
                tab->keyring = realloc(tab->keyring, newsize);
        }
        tab->keyring[tab->n] = mydup(key);

        return (tab->n++);
}



// Create minimal perfect hash function from in-memory vector of strings.
unsigned int mph_gen(struct mph_t *tab) 
{
        assert(tab != NULL);

        unsigned int i;
        FILE *mphf_fd;  // Where the generated hash function will be stored.

        cmph_io_adapter_t *source;
        cmph_config_t     *config;
        cmph_t            *hash;

        mphf_fd = fopen(tab->fn, "w");

        // Source of keys
        source = cmph_io_vector_adapter((char **)tab->keyring, tab->n);

        //Create minimal perfect hash function using the brz algorithm.
        config = cmph_config_new(source);
        cmph_config_set_algo(config, CMPH_BRZ);
        cmph_config_set_mphf_fd(config, mphf_fd);

        hash = cmph_new(config);

        cmph_config_destroy(config);
        cmph_dump(hash, mphf_fd); 
        cmph_destroy(hash);	
        fclose(mphf_fd);

        return 0;
}



// Returns the index of string 'key'
unsigned int mph_ind(struct mph_t *tab, const char *key)
{
        assert(key != NULL);

        unsigned int id;
        FILE *mphf_fd;
        cmph_t *hash;

        mphf_fd = fopen(tab->fn, "r");
        hash    = cmph_load(mphf_fd);
        id      = cmph_search(hash, key, (cmph_uint32)strlen(key));

        fclose(mphf_fd);

        return (id);
}

/*int mph_destroy(void)*/
/*{*/
        /*//Destroy hash*/
        /*cmph_destroy(hash);*/
        /*cmph_io_vector_adapter_destroy(source);   */
        /*fclose(mphf_fd);*/
/*}*/
