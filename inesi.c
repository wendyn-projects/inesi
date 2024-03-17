#include "inesi.h"

#include <stddef.h>
#include <string.h>

static inline bool parseINES_Archaic(uint8_t const arHeader[static 15], INES_HeaderInfo rrInfo[static 1])
{
    rrInfo->mHeaderType = INES_HT_INES_ARCHAIC;
    rrInfo->mPRG_ROM_B <<= 14;
    rrInfo->mCHR_ROM_B <<= 13;
    rrInfo->mHeaderTypeVariant.INES_HT_INES_ARCHAIC.mMapper = (arHeader[5] & 0xF0) >> 4;
    int8_t * lrAuthorCursor = rrInfo->mHeaderTypeVariant.INES_HT_INES_ARCHAIC.mAuthor;
    for (size_t i = 6; i < 15; i++)
    {
        if (arHeader[i] == 0)
            continue;
        *lrAuthorCursor++ = arHeader[i];
    }
    *lrAuthorCursor = '\0';
    return true;
}

static inline bool parse_INES(uint8_t const arHeader[static 15], INES_HeaderInfo rrInfo[static 1])
{
    rrInfo->mHeaderType = INES_HT_INES;
    rrInfo->mPRG_ROM_B <<= 14;
    rrInfo->mCHR_ROM_B <<= 13;
    struct INES_HT_INES_t * const lrINES = &rrInfo->mHeaderTypeVariant.INES_HT_INES;
    lrINES->mMapper = (arHeader[5] & 0xF0) >> 4;
    lrINES->mSystem = (arHeader[6] & (1 << 0)) == 1? INES_S_VS_SYSTEM : INES_S_NES;
    lrINES->mMapper |= (arHeader[6] & 0xF0);
    lrINES->mPRG_RAM_8kB_Banks = arHeader[7] + 1;
    lrINES->mTV_System = (arHeader[8] & (1 << 0)) == 1? INES_TVS_PAL : INES_TVS_NTSC;
    int8_t * lrAuthorCursor = lrINES->mAuthor;
    for (size_t i = 9; i < 15; i++)
    {
        if (arHeader[i] == 0)
            continue;
        *lrAuthorCursor++ = arHeader[i];
    }
    *lrAuthorCursor = '\0';
    return true;
}

static inline uint64_t calculateNES_2_0_ROM(uint16_t aValue, uint16_t aMultiplier)
{
    if ((aValue & 0x0F00) != 0x0F00)
        return (uint64_t)aValue * aMultiplier;

    unsigned int lMultiplier = (aValue & 0x03) * 2 + 1;
    unsigned int lExponent = ((uint8_t)aValue) >> 2;

    if (lMultiplier == 63 && lMultiplier > 2)
        return INES_ROM_SIZE_TOO_BIG;

    return ((uint64_t)1 << lExponent) * lMultiplier;
}

static inline bool parseNES_2_0(uint8_t const arHeader[static 15], INES_HeaderInfo rrInfo[static 1])
{
    rrInfo->mHeaderType = INES_HT_NES_2_0;
    struct INES_HT_NES_2_0_t * const lrNES_2_0 = &rrInfo->mHeaderTypeVariant.INES_HT_NES_2_0;
    lrNES_2_0->mMapper = (arHeader[5] & 0xF0) >> 4;
    switch (arHeader[6] & 0x03)
    {
    case 0x00:
        lrNES_2_0->mSystem = INES_S_NES;
        break;
    case 0x01:
        lrNES_2_0->mSystem = INES_S_VS_SYSTEM;
        break;
    case 0x02:
        lrNES_2_0->mSystem = INES_S_PLAYCHOICE_10;
        break;
    case 0x03:
        lrNES_2_0->mSystem = INES_S_OTHER;
        break;
    }
    lrNES_2_0->mMapper |= (arHeader[6] & 0xF0);
    lrNES_2_0->mMapper |= (arHeader[7] & 0x0F) << 8;
    lrNES_2_0->mMapperVariant = (arHeader[7] & 0xF0) >> 4;

    uint16_t lROM_Value = ((arHeader[8] & 0x0F) << 8) | (rrInfo->mPRG_ROM_B / 2);
    rrInfo->mPRG_ROM_B = calculateNES_2_0_ROM(lROM_Value, 1 << 14);
    lROM_Value = ((arHeader[8] & 0xF0) << 4) | rrInfo->mCHR_ROM_B;
    rrInfo->mCHR_ROM_B = calculateNES_2_0_ROM(lROM_Value, 1 << 13);

    lrNES_2_0->mPRG_VolatileRAM_B = (64 << (arHeader[9] & 0xF));
    if (lrNES_2_0->mPRG_VolatileRAM_B == 64)
        lrNES_2_0->mPRG_VolatileRAM_B = 0;

    lrNES_2_0->mPRG_NonvolatileRAM_B = (64 << (arHeader[9] >> 4));
    if (lrNES_2_0->mPRG_NonvolatileRAM_B == 64)
        lrNES_2_0->mPRG_NonvolatileRAM_B = 0;

    lrNES_2_0->mCHR_VolatileRAM_B = (64 << (arHeader[10] & 0xF));
    if (lrNES_2_0->mCHR_VolatileRAM_B == 64)
        lrNES_2_0->mCHR_VolatileRAM_B = 0;

    lrNES_2_0->mCHR_NonvolatileRAM_B = (64 << (arHeader[10] >> 4));
    if (lrNES_2_0->mCHR_NonvolatileRAM_B == 64)
        lrNES_2_0->mCHR_NonvolatileRAM_B = 0;

    switch(arHeader[11] & 0x03)
    {
    case 0:
        lrNES_2_0->mTiming = INES_T_NTSC;
        break;
    case 1:
        lrNES_2_0->mTiming = INES_T_LICENSED_PAL;
        break;
    case 2:
        lrNES_2_0->mTiming = INES_T_MULTIREGION;
        break;
    case 3:
        lrNES_2_0->mTiming = INES_T_DENDY;
        break;
    }

    switch(lrNES_2_0->mSystem)
    {
    case INES_S_VS_SYSTEM: {
            struct INES_S_VS_SYSTEM_t * const lVS_System = &lrNES_2_0->mSystemVariant.INES_S_VS_SYSTEM;
            lVS_System->mHardwareType = arHeader[12] >> 4;
            lVS_System->mPPU_Type = arHeader[12] & 0x0F;
            break;
        }
    case INES_S_OTHER: {
            struct INES_S_OTHER_t * const lOtherSystem = &lrNES_2_0->mSystemVariant.INES_S_OTHER;
            lOtherSystem->mType = arHeader[12] >> 4;
            break;
        }
    default:
        break;
    }

    lrNES_2_0->mMiscellaneousROM_Count = arHeader[13] & 0x03;
    lrNES_2_0->mDefaultExpansionDevice = arHeader[14] & 0x3F;

    return true;
}

bool parseHeader(uint8_t const arHeader[static 15], INES_HeaderInfo rrInfo[static 1])
{
    static uint8_t const lMagicNumber[4] = { 0x4E, 0x45, 0x53, 0x1A };
    for (size_t i = 0; i < sizeof(lMagicNumber) / sizeof(*lMagicNumber); i++)
    {
        if (arHeader[i] != lMagicNumber[i])
            goto failed;
    }

    rrInfo->mPRG_ROM_B = arHeader[4];

    rrInfo->mCHR_ROM_B = arHeader[5];

    rrInfo->mNametableArrangement = (arHeader[6] & (1 << 0)) == 1? INES_NA_VERTICAL : INES_NA_HORIZONTAL;
    rrInfo->mHasBattery = arHeader[6] & (1 << 1);
    rrInfo->mHasTrainer = arHeader[6] & (1 << 2);
    rrInfo->mHasAlternativeNametableLayout = arHeader[6] & (1 << 3);

    switch (arHeader[6] & 0x0C)
    {
    case 0x00:
        if (parse_INES(arHeader, rrInfo))
            return true;
        goto failed;
    case 0x04:
        if (parseINES_Archaic(arHeader, rrInfo))
            return true;
        goto failed;
    case 0x08:
        if (parseNES_2_0(arHeader, rrInfo))
            return true;
        goto failed;
    default:
        goto failed;
    }

failed:
    memset(rrInfo, 0, sizeof(*rrInfo));
    return false;
#undef read_byte
}