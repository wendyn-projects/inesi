#include <assert.h>
#include <stdio.h>

#include "inesi.h"

static void printMemorySize(uint64_t aMemorySize);
static void printNameTableArrangement(INES_NametableArrangement aArrangement);
static void printHeaderType(INES_HeaderType aHeaderType);
static inline void printAuthor(int8_t const aCStr[static 1]);
static void printTVSystem(INES_TV_System aTVSystem);
static void printSystem(INES_System aSystem);
static void printTiming(INES_Timing aTiming);

int main(int argc, char** argv)
{
    assert(argc >= 2);
    FILE* lpStream;
    assert(lpStream = fopen(argv[1], "rb"));
    uint8_t lHeaderBytes[15];
    assert(fread(lHeaderBytes, sizeof(lHeaderBytes), 1, lpStream) == 1);
    assert(fclose(lpStream) != EOF);
    INES_HeaderInfo lHeader = { 0 };
    assert(parseHeader(lHeaderBytes, &lHeader));

    printf("PRG_ROM_SIZE = ");
    printMemorySize(lHeader.mPRG_ROM_B);
    puts("");

    printf("CHR_ROM_SIZE = \"");
    printMemorySize(lHeader.mCHR_ROM_B);
    puts("\"");

    printf("NAME_TABLE_ARRANGEMENT = \"");
    printNameTableArrangement(lHeader.mNametableArrangement);
    puts("\"");

    printf("HAS_BATTERY = ");
    printf("%s", lHeader.mHasBattery? "true" : "false");
    puts("");

    printf("HAS_TRAINER = ");
    printf("%s", lHeader.mHasTrainer? "true" : "false");
    puts("");

    printf("HAS_ALTERNATIVE_NAMETABLE_LAYOUT = ");
    printf("%s", lHeader.mHasAlternativeNametableLayout? "true" : "false");
    puts("");

    printf("HEADER_TYPE = \"");
    printHeaderType(lHeader.mHeaderType);
    puts("\"");

    switch(lHeader.mHeaderType)
    {
    case INES_HT_INES_ARCHAIC: {
        struct INES_HT_INES_ARCHAIC_t const * const lrNES_Archaic = &lHeader.mHeaderTypeVariant.INES_HT_INES_ARCHAIC;
        printf("MAPPER = ");
        printf("0x%02X", lrNES_Archaic->mMapper);
        puts("");

        printf("AUTHOR = \"");
        printAuthor(lrNES_Archaic->mAuthor);
        puts("\"");
        break;
    }
    case INES_HT_INES: {
            struct INES_HT_INES_t const * const lrNES = &lHeader.mHeaderTypeVariant.INES_HT_INES;
            printf("MAPPER = ");
            printf("0x%02X", lrNES->mMapper);
            puts("");

            printf("TV_SYSTEM = \"");
            printTVSystem(lrNES->mTV_System);
            puts("\"");

            printf("SYSTEM = \"");
            printSystem(lrNES->mSystem);
            puts("\"");

            printf("PRG_RAM_SIZE = \"");
            printMemorySize(lrNES->mPRG_RAM_8kB_Banks << 13);
            puts("\"");

            printf("AUTHOR = \"");
            printAuthor(lrNES->mAuthor);
            puts("\"");
            break;
        }
    case INES_HT_NES_2_0: {
            struct INES_HT_NES_2_0_t const * const lrNES_2_0 = &lHeader.mHeaderTypeVariant.INES_HT_NES_2_0;
            printf("MAPPER = ");
            printf("0x%04X", lrNES_2_0->mMapper);
            puts("");

            printf("MAPPER_VARIANT = ");
            printf("0x%02X", lrNES_2_0->mMapperVariant);
            puts("");

            printf("TV_SYSTEM = \"");
            printTVSystem(lrNES_2_0->mTV_System);
            puts("\"");

            printf("SYSTEM = \"");
            printSystem(lrNES_2_0->mSystem);
            puts("\"");

            switch(lrNES_2_0->mSystem)
            {
            case INES_S_VS_SYSTEM: {
                    struct INES_S_VS_SYSTEM_t const * const lVS_System = &lrNES_2_0->mSystemVariant.INES_S_VS_SYSTEM;
                    printf("VS_SYSTEM.HARDWARE_TYPE = ");
                    printf("0x%02X", lVS_System->mHardwareType);
                    puts("");

                    printf("VS_SYSTEM.PPU_TYPE = ");
                    printf("0x%02X", lVS_System->mPPU_Type);
                    puts("");
                    break;
                }
            case INES_S_OTHER: {
                    struct INES_S_OTHER_t const * const lOtherSystem = &lrNES_2_0->mSystemVariant.INES_S_OTHER;
                    printf("OTHER.TYPE = ");
                    printf("0x%02X", lOtherSystem->mType);
                    puts("");
                    break;
                }
            default:
                break;
            }
            printf("PRG_VOLATILE_RAM_SIZE = \"");
            printMemorySize(lrNES_2_0->mPRG_VolatileRAM_B);
            puts("\"");

            printf("PRG_NONVOLATILE_RAM_SIZE = \"");
            printMemorySize(lrNES_2_0->mPRG_NonvolatileRAM_B);
            puts("\"");

            printf("CHR_VOLATILE_RAM_SIZE = \"");
            printMemorySize(lrNES_2_0->mCHR_VolatileRAM_B);
            puts("\"");

            printf("CHR_NONVOLATILE_RAM_SIZE = \"");
            printMemorySize(lrNES_2_0->mCHR_NonvolatileRAM_B);
            puts("\"");

            printf("TIMING = \"");
            printTiming(lrNES_2_0->mTiming);
            puts("\"");

            printf("MISCELLALENOUS_ROM_COUNT = ");
            printf("%hhd", lrNES_2_0->mMiscellaneousROM_Count);
            puts("");

            printf("DEFAULT_EXPANSION_DEVICE = ");
            printf("%hhd", lrNES_2_0->mDefaultExpansionDevice);
            puts("");
            break;
        }
    default:
        bool lImplemented = false;
        assert(lImplemented);
    }

    return 0;
}

static void printMemorySize(uint64_t aMemorySize)
{
    if (aMemorySize == INES_ROM_SIZE_TOO_BIG)
    {
        printf("too big");
        return;
    }
    uint64_t const cMultiplierStep = 1024;
    uint64_t lMultiplier = 1024;
    uint64_t lMultiplierCount = 0;

    while (aMemorySize / lMultiplier != 0 && aMemorySize % lMultiplier == 0)
    {
        ++lMultiplierCount;
        lMultiplier *= cMultiplierStep;
    }
    lMultiplier /= cMultiplierStep;

    char const * lUnits[] = { "B", "kiB", "MiB", "GiB", "TiB" };

    if (lMultiplierCount >= sizeof(lUnits) / sizeof(*lUnits))
    {
        lMultiplier = 1;
        lMultiplierCount = 0;
    }

    printf("%llu %s", (long long unsigned int)aMemorySize / lMultiplier, lUnits[lMultiplierCount]);
}

static void printNameTableArrangement(INES_NametableArrangement aArrangement)
{
    switch(aArrangement)
    {
    case INES_NA_HORIZONTAL:
        printf("horizontal");
        return;
    case INES_NA_VERTICAL:
        printf("vertical");
        return;
    }
}

static void printHeaderType(INES_HeaderType aHeaderType)
{
    char const * cHeaderTypesNames[] = {
        [INES_HT_UNKNOWN] = "unknown",
        [INES_HT_INES_ARCHAIC] = "INES (Archaic)",
        [INES_HT_INES] = "INES",
        [INES_HT_NES_2_0] = "NES 2.0",
    };
    assert(aHeaderType < sizeof(cHeaderTypesNames) / sizeof(*cHeaderTypesNames));

    printf("%s", cHeaderTypesNames[aHeaderType]);
}

static inline void printAuthor(int8_t const aCStr[static 1])
{
    for (int8_t const * lCursor = aCStr; *lCursor != '\0'; lCursor++)
        putchar(*lCursor < 32 || *lCursor > 127? '?' : *lCursor);
}

static void printTVSystem(INES_TV_System aTVSystem)
{
    switch(aTVSystem)
    {
    case INES_TVS_NTSC:
        printf("NTSC");
        return;
    case INES_TVS_PAL:
        printf("PAL");
        return;
    }
}

static void printSystem(INES_System aSystem)
{
    char const * cSystemsNames[] = {
        [INES_S_NES] = "NES",
        [INES_S_VS_SYSTEM] = "Vs. System",
        [INES_S_PLAYCHOICE_10] = "Playchoice 10",
        [INES_S_OTHER] = "Other",
    };
    assert(aSystem < sizeof(cSystemsNames) / sizeof(*cSystemsNames));

    printf("%s", cSystemsNames[aSystem]);
}

static void printTiming(INES_Timing aTiming)
{
    char const * cTimingsNames[] = {
        [INES_T_NTSC] = "NTSC",
        [INES_T_LICENSED_PAL] = "Licensed PAL",
        [INES_T_MULTIREGION] = "Multi-regional",
        [INES_T_DENDY] = "Dendy",
    };
    assert(aTiming < sizeof(cTimingsNames) / sizeof(*cTimingsNames));

    printf("%s", cTimingsNames[aTiming]);
}