#ifndef INESI_H_
#define INESI_H_

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>

#define INES_ROM_SIZE_TOO_BIG UINT64_MAX

typedef enum {
    INES_NA_HORIZONTAL = 0,
    INES_NA_VERTICAL = 1
} INES_NametableArrangement;

typedef enum {
    INES_TVS_NTSC = 0,
    INES_TVS_PAL = 1
} INES_TV_System;

typedef enum {
    INES_S_NES,
    INES_S_VS_SYSTEM,
    INES_S_PLAYCHOICE_10,
    INES_S_OTHER,
} INES_System;

typedef enum {
    INES_HT_UNKNOWN,
    //INES_HT_TNES,
    INES_HT_INES_ARCHAIC,
    //INES_HT_INES_0_7,
    INES_HT_INES,
    INES_HT_NES_2_0,
} INES_HeaderType;

typedef enum {
    INES_T_NTSC,
    INES_T_LICENSED_PAL,
    INES_T_MULTIREGION,
    INES_T_DENDY,
} INES_Timing;

typedef struct {
    uint64_t mPRG_ROM_B;
    uint64_t mCHR_ROM_B;
    INES_NametableArrangement mNametableArrangement;
    bool mHasBattery;
    bool mHasTrainer;
    bool mHasAlternativeNametableLayout;

    INES_HeaderType mHeaderType;
    union {
        struct INES_HT_INES_ARCHAIC_t {
            uint32_t mMapper;
            int8_t mAuthor[10];
        } INES_HT_INES_ARCHAIC;

        struct INES_HT_INES_t {
            uint32_t mMapper;
            INES_TV_System mTV_System;
            INES_System mSystem;
            uint16_t mPRG_RAM_8kB_Banks;
            int8_t mAuthor[7];
        } INES_HT_INES;

        struct INES_HT_NES_2_0_t {
            uint32_t mMapper;
            INES_TV_System mTV_System;
            INES_System mSystem;
            union {
                struct INES_S_VS_SYSTEM_t {
                    uint8_t mPPU_Type;
                    uint8_t mHardwareType;
                } INES_S_VS_SYSTEM;
                struct INES_S_OTHER_t {
                    uint8_t mType;
                } INES_S_OTHER;
            } mSystemVariant;
            uint16_t mPRG_VolatileRAM_B;
            uint16_t mPRG_NonvolatileRAM_B;
            uint16_t mCHR_VolatileRAM_B;
            uint16_t mCHR_NonvolatileRAM_B;
            uint8_t mMapperVariant;
            INES_Timing mTiming;
            uint8_t mMiscellaneousROM_Count;
            uint8_t mDefaultExpansionDevice;
        } INES_HT_NES_2_0;
    } mHeaderTypeVariant;
} INES_HeaderInfo;

bool parseHeader(uint8_t const arHeader[static 15], INES_HeaderInfo rrInfo[static 1]);

#endif