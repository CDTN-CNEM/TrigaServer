struct SPU_DATA
{
    bool    READ            = 0;
    float   N_DATA_FP       = 0;
    float   T_DATA_FP       = 0;
    float   F1_DATA_FP      = 0;
    float   F2_DATA_FP      = 0;
    float   F3_DATA_FP      = 0;
    float   EMR_N_THRESHOLD = 0;
    float   WRN_N_THRESHOLD = 0;
    float   EMR_T_THRESHOLD = 0;
    float   WRN_T_THRESHOLD = 0;
    bool    EMR_N           = 0;
    bool    WRN_N           = 0;
    bool    EMR_T           = 0;
    bool    WRN_T           = 0;
    bool    R1              = 0;
    bool    R2              = 0;
    bool    R3              = 0;
    bool    RDY             = 0;
    bool    TEST            = 0;
    bool    XXXX            = 0;
};

struct PLC_DATA
{
    bool    READ            = 0;
    float   CH_PAR          = 0;
    float   CH_LOG          = 0;
    float   CH_LIN          = 0;
    float   CH_PER          = 0;
    
};

struct ALL_DATA
{
    PLC_DATA PLC;
    SPU_DATA SPU_CHA;
    SPU_DATA SPU_CHB;
};