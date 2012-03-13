/*|  \ \ | |/ /
  |  |\ `' ' /
  |  ;'      \      / ,          
  | ;    _,   |    / / ,          
  | |   (  `-.;_,-' '-' ,
  | `,   `-._       _,-'_
  |,-`.    `.)    ,<_,-'_,          
 ,'    `.   /   ,'  `;-' _,       
;        `./   /`,    \-'
|         /   |  ;\   |\
|        ;_,._|_,  `, ' \               vitals.h
|        \    \ `       `,
`      __ `    \         ;,
 \   ,'  `      \,       ;,    
  \_(            ;,      ;;
  |  \           `;,     ;;
  |  |`.          `;;,   ;'
  |  |  `-.        ;;;;,;'   
  |  |    |`-.._  ,;;;;;'
  |  |    |   | ``';;;'
*/

#include <stdint.h>
#include <stdbool.h>

static const int VIT_NVITALS=4;
static const int VIT_NSTATES=64;
static const int VIT_MAXLEN=30;
/* Each block of the vitals bar is a multiple of the actual value of the vitals
 * stat. The size of the multiple is the "grain". */
static const int VIT_GRAIN=24;

enum VIT_TAG_BLOCKS { __HP__ , __SP__ , __LP__ , __EP__  };

static const int VIT_MAX_BLOCK[]={5, 8, 5, 3};
static const int VIT_MIN_BLOCK[]={1, 1, 1, 1};


/* Lookup table for each possible state of a single pkg. */
static const uint32_t state[]={
        0x00000000, 0x00000001, 0x00000002, 0x00000003,
        0x00000004, 0x00000005, 0x00000006, 0x00000007,
        0x00000008, 0x00000009, 0x0000000A, 0x0000000B,
        0x0000000C, 0x0000000D, 0x0000000E, 0x0000000F,

        0x00000010, 0x00000011, 0x00000012, 0x00000013,
        0x00000014, 0x00000015, 0x00000016, 0x00000017,
        0x00000018, 0x00000019, 0x0000001A, 0x0000001B,
        0x0000001C, 0x0000001D, 0x0000001E, 0x0000001F,

        0x00000020, 0x00000021, 0x00000022, 0x00000023,
        0x00000024, 0x00000025, 0x00000026, 0x00000027,
        0x00000028, 0x00000029, 0x0000002A, 0x0000002B,
        0x0000002C, 0x0000002D, 0x0000002E, 0x0000002F,

        0x00000030, 0x00000031, 0x00000032, 0x00000033,
        0x00000034, 0x00000035, 0x00000036, 0x00000037,
        0x00000038, 0x00000039, 0x0000003A, 0x0000003B,
        0x0000003C, 0x0000003D, 0x0000003E, 0x0000003F
};

/* Normal and inverted masks for each of the 4 nibbles. */
static const uint32_t scrub[]={0x00FFFFFF, 0xFF00FFFF, 0xFFFF00FF, 0xFFFFFF00};
static const uint32_t gouge[]={0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF};

/* IMPORTANT: These bit offsets assume litte-endianness */
static const uint32_t offset[]={24, 16, 8, 0};

//////////////////////////////////////////////
//            Health       Lucidity         //
//                \        /                //            
enum vitals_enum {HP, SP, LP, EP};          //
//                    |        \            //
//                  Stamina    Emotion      //
//////////////////////////////////////////////

static inline void set_vital(uint32_t *vpkg, int n, int s)
{
        uint32_t c = *vpkg; // copy

        c &= scrub[n];
        c |= (state[s]<<offset[n]);

        *vpkg = c;
}

static inline int get_vital(uint32_t *vpkg, int n)
{
        uint32_t c = *vpkg; // copy

        c &= ~(scrub[n]);
        c >>= offset[n];
        return (int)(c);
}

static inline void vit_unpack(uint32_t *vpkg, int *dest, size_t n)
{
        if (dest==NULL || vpkg==NULL || n<VIT_NVITALS) return;
        int i;
        for (i=0; i<VIT_NVITALS; i++)
                dest[i]=get_vital(vpkg, i); 
}


static inline void inc_vital(uint32_t *v, int n)
{
        set_vital(v, n, (get_vital(v, n))+1);
}
static inline void dec_vital(uint32_t *v, int n)
{
        set_vital(v, n, (get_vital(v, n))-1);
}

static inline int vit_blocklen(uint32_t vpkg)
{
        int vital[VIT_NVITALS]; // array to hold unpacked vitals
        int accumulate=0;
        int i;

        vit_unpack(&vpkg, vital, VIT_NVITALS);

        for (i=0; i<VIT_NVITALS; i++) {
                accumulate += vital[i];
        }

        return accumulate;
}

void vit_print_blocks(WINDOW *win, int xofs, uint32_t key);

void vit_dopulse(void);

int vit_blocklen(uint32_t vpkg);
