/******************************/
/* MACROS FOR COROUTINES IN C */
/******************************/
#define crSTART static int state=0; switch(state) { case 0:
#define crRETURN(x) do { state=__LINE__; return x; \
                         case __LINE__:; } while (0)
#define crFINISH }
