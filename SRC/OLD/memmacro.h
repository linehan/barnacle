/******************************************************************************
 * Macros to simplify memory allocation operations                            *
 ******************************************************************************/

/* allocates a single object using malloc(). */
#define MALLOC(type) ((type *)malloc(sizeof(type)))
 
/* allocates an array of objects using malloc(). */
#define MALLOC_ARRAY(number, type) \
    ((type *)malloc((number) * sizeof(type)))
 
/* allocates a single object with a flexible
 * array member using malloc(). */
#define MALLOC_FLEX(stype, number, etype) \
    ((stype *)malloc(sizeof(stype) \
    + (number) * sizeof(etype)))
 
/* allocates an array of objects using calloc(). */
#define CALLOC(number, type) \
    ((type *)calloc(number, sizeof(type)))
 
/* reallocates an array of objects using realloc(). */
#define REALLOC_ARRAY(pointer, number, type) \
    ((type *)realloc(pointer, (number) * sizeof(type)))
 
/* reallocates a single object with a flexible
 * array member using realloc(). */
#define REALLOC_FLEX(pointer, stype, number, etype) \
    ((stype *)realloc(pointer, sizeof(stype) \
    + (number) * sizeof(etype)))
