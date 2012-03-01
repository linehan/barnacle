#define NAMES_HUMAN_ENUM names_human_enum
#define NAMES_HUMAN_CHAR names_human_char
#define NAMES_HUMAN_LIST    \
        TWINDEX(FIRST,  0)  \
        TWINDEX(MIDDLE, 1)  \
        TWINDEX(LAST,   2)  \
        TWINDEX(NICK,   3)  \
        TWINDEX(HONOR,  4)  \ /* e.g. His royal loftiness..." */\
        TWINDEX(TITLE,  5)    /* e.g. Dr., Lieutenant, etc. */

#define ATTRIBUTES_HUMAN_ENUM attributes_human_enum
#define ATTRIBUTES_HUMAN_CHAR attributes_human_char
#define ATTRIBUTES_HUMAN_LIST \
        TWINDEX(GENDER, 0)    \
        TWINDEX(AGE,    1)    \
        TWINDEX(WEIGHT, 2)    \
        TWINDEX(HEIGHT, 3)


#define TRADES_HUMAN_ENUM trades_human_enum
#define TRADES_HUMAN_CHAR trades_human_char 
#define TRADES_HUMAN_LIST      \
        TWINDEX(ABLE_SEAMAN  , 0)    \
        TWINDEX(ACCOUNTANT   , 1)    \
        TWINDEX(ACTOR        , 2)    \
        TWINDEX(ARISTOCRAT   , 3)    \
        TWINDEX(ASTRONOMER   , 4)    \
        TWINDEX(BARBER       , 5)    \
        TWINDEX(BEGGAR       , 6)    \
        TWINDEX(BLACKSMITH   , 7)    \
        TWINDEX(BOOKKEEPER   , 8)    \
        TWINDEX(BOTANIST     , 9)    \
        TWINDEX(BUTCHER      ,10)    \
        TWINDEX(CARPENTER    ,11)    \
        TWINDEX(CHEMIST      ,12)    \
        TWINDEX(CLERGYMAN    ,13)    \
        TWINDEX(COOPER       ,14)    \
        TWINDEX(COPPICER     ,15)    \
        TWINDEX(DIVER        ,16)    \
        TWINDEX(DRIVER       ,17)    \
        TWINDEX(ENGINEER     ,18)    \
        TWINDEX(FARMER       ,19)    \
        TWINDEX(GROCER       ,20)    \
        TWINDEX(HIGHWAYMAN   ,21)    \
        TWINDEX(HOROLOGIST   ,22)    \
        TWINDEX(INNKEEPER    ,23)    \
        TWINDEX(INVENTOR     ,24)    \
        TWINDEX(LINGUIST     ,25)    \
        TWINDEX(LONGSHOREMAN ,26)    \
        TWINDEX(MONK         ,27)    \
        TWINDEX(MUSICIAN     ,28)    \
        TWINDEX(NAVIGATOR    ,29)    \
        TWINDEX(OFFICER_ARMY ,30)    \
        TWINDEX(OFFICER_NAVY ,31)    \
        TWINDEX(OPTICIAN     ,32)    \
        TWINDEX(PAINTER      ,32)    \
        TWINDEX(PHILOSOPHER  ,34)    \
        TWINDEX(PILOT        ,35)    \
        TWINDEX(PLANTER      ,36)    \
        TWINDEX(POET         ,37)    \
        TWINDEX(PRIEST       ,38)    \
        TWINDEX(PROSTITUTE   ,39)    \
        TWINDEX(SCHOOLMASTER ,40)    \
        TWINDEX(SCRIBE       ,41)    \
        TWINDEX(SERVANT      ,42)    \
        TWINDEX(SHEPHERD     ,43)    \
        TWINDEX(SHOPKEEPER   ,44)    \
        TWINDEX(SLAVE        ,45)    \
        TWINDEX(SOLDIER      ,46)    \
        TWINDEX(SURGEON      ,47)    \
        TWINDEX(SURVEYOR     ,48)    \
        TWINDEX(TAILOR       ,49)    \
        TWINDEX(THIEF        ,50)    \
        TWINDEX(WEAVER       ,51)


// Each attribute takes half a nibble.
uint32_t human_attr;
enum human_attr { 
       STR=0,    DEX=2,     VIT=4,     AGI=6,   INT=8,        CHA=10, 
//     Strength  Dexterity  Vitality   Agility  Intelligence  Charm
       TRA=12,   WEA=14,    EDU=16,    MAL=18,  BEN=20, 
//     Travel    Wealth     Education  Malice   Benificence
       EAR=22,   EYE=24,    NOS=26,    TOU=28,  TAS=30 
//     Hearing   Sight      Smell      Touch    Taste
};

struct human {
        uint32_t key;
        uint32_t attr;
        const char **name[6];
        unsigned char age;
        unsigned char gender;
        unsigned char trade;
        unsigned char origin;
        unsigned char ethnic;
        unsigned char guild;
        unsigned char hair;
        unsigned char eyes;
        unsigned char weight;
        unsigned char height;
};
