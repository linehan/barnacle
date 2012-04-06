#ifndef __ID_TYPE
#define __ID_TYPE
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%                                                                            % 
//%  The id_t encapsulates two quantities that can together retreive           %
//%  a unique record and associate it with its legal operations.               % 
//%                                                                            % 
//%   "key" is a unique identifier used to retreive the record                 % 
//%         from a data structure, here a red-black tree.                      % 
//%                                                                            % 
//%  "kind" represents the record semantically, i.e., it qualifies             % 
//%         the record for a subset of operations once retreived.              % 
//%                                                                            % 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
struct id_t {
        uint32_t key;
        unsigned char kind;
};
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


uint32_t LAST_KEY; // Incremented by new_id();


static struct id_t *new_id(unsigned char kind)
{
        struct id_t *new = malloc(sizeof(*new));
        new->kind = kind;
        new->key = LAST_KEY++;

        return (new);
}

#endif
