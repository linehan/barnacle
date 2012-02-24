/*
    macross.h 

        Description
        Defines a special macro construction that allows you to create 
        flexible kinds of objects using the storage system.

        Usage
        This header requires that you provide a pair of preprocessor
        directives in the source file which includes it.

        Those directives are:

                #define TYPE_FOR_<MYITEMID>
                #define ALLO_FOR_<MYITEMID>

        where <MYITEMID> is an identifying string token of your choice,
        used in a consistent way (more later).

        Predictably, TYPE_FOR_<MYITEMID> should provide the type of
        item the constructor is going to produce. It should be the
        base type, not a pointer, unless it is a pointer-to-a-pointer,
        etc., because the product of our constructor is going to be
        dynamically allocated (see below).

        ALLO_FOR_<MYITEMID> should expand to the allocation routine
        that will allow the product (of type TYPE_FOR_<MYITEMID>) to
        be dynamically allocated properly.

        These are the required "book-keeping" directives, but in order
        to actually construct anything, you will have to define various
        classes of constructor, like so:

                #define MAKE_<MYITEMID>_<MYCLASSID>(name)

        This directive should expand into a generic instance of the
        registered constructor routine. Then, we can declare a custom
        item like so:

                MAKE_MY(<MYITEMID>, name, <MYCLASSID>)

        The statement MAKE_MY(...) will expand using the macro defined
        below, concatenating (using the ## token) the <MYITEMID> and
        <MYCLASSID> to create the unique directives, which will then
        expand based on their definitions in the source file.

        Here is an example.

        Say we decide to make a pony, but after considering it for a
        little while, we can't make up our minds whether we want a
        brown pony or a palomino pony. We want it all, and that's ok.

        In pony.c, we type the following:


                #include "macross.h"

                struct prettypony {
                        int brown;
                        int palomino;
                };

                #define TYPE_FOR_PONY struct prettypony
                #define ALLO_FOR_PONY malloc(sizeof(struct prettypony))


        So far, so good. Now we need to define how to build a brown or
        palomino pony.

                #define MAKE_PONY_BROWN(name) \
                        name->brown = 0;      \
                        name->palomino = 1;

                #define MAKE_PONY_PALOMINO(name) \
                        name->brown = 1;         \
                        name->palomino = 0;

        Simple enough. Now, to declare a pony of our choice, we simply
        use the statement
                
                MAKE_MY(PONY, BROWN, heather);

        Which will expand into

                struct prettypony *heather = malloc(sizeof(struct prettypony));
                heather->brown = 0;
                heather->palomino = 1;

        Happy trails!
*/




#define MAKE_MY(item, name, kind) \
        TYPE_FOR_ ## item *name = ALLO_FOR_ ## item \
        MAKE_ ## item ## _ ## kind (name)

