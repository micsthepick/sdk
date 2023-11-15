// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <stdio.h>

#include <nds.h>

#include "teak_tlf_bin.h"

int main(int argc, char **argv)
{
    consoleDemoInit();

    if (!isDSiMode())
    {
        printf("DSP only available on DSi");
        while (1)
            swiWaitForVBlank();
    }

    if (!dspExecuteDefaultTLF(teak_tlf_bin))
    {
        printf("Failed to execute TLF");
        while (1)
            swiWaitForVBlank();
    }

    uint16_t rep0 = 0;
    uint16_t rep1 = 0;
    uint16_t rep2 = 0;

    uint16_t cmd0 = 0;

    int count = 0;

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        uint16_t keys = keysHeld();
        if (keys & KEY_START)
            break;

        printf("\x1b[2J"); // Clear console

        // Heartbeat counter

        cmd0 = count++;

        printf("A: Send message to channel 1\n");
        printf("B: Send message to channel 2\n");
        printf("\n");

        printf("CMD: %u\n\n", cmd0);

        if (dspSendDataReady(0))
            dspSendData(0, cmd0);

        if (keys & KEY_A)
        {
            if (dspSendDataReady(1))
                dspSendData(1, 0);
        }

        if (keys & KEY_B)
        {
            if (dspSendDataReady(2))
                dspSendData(2, 0);
        }

        if (dspReceiveDataReady(0))
            rep0 = dspReceiveData(0);

        // Get counters

        if (dspReceiveDataReady(1))
            rep1 = dspReceiveData(1);
        if (dspReceiveDataReady(2))
            rep2 = dspReceiveData(2);

        printf("REP: %u 0x%04X 0x%04X\n\n", rep0, rep1, rep2);

        printf("Press START to exit");
    }

    return 0;
}