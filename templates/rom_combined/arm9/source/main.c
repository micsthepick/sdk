// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <stdio.h>
#include <time.h>

#include <nds.h>
#include <maxmod9.h>

#include "common.h"

// Headers autogenerated when files are find inside AUDIODIRS in the Makefile
#include "soundbank.h"
#include "soundbank_bin.h"

// Header autogenerated for each PNG file inside GFXDIRS in the Makefile
#include "neon.h"

// Header autogenerated for each BIN file inside BINDIRS in the Makefile
#include "data_string_bin.h"

int main(int argc, char **argv)
{
    int textureID;

    videoSetMode(MODE_0_3D);

    glInit();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_ANTIALIAS);

    // The background must be fully opaque and have a unique polygon ID
    // (different from the polygons that are going to be drawn) so that
    // antialias works.
    glClearColor(0, 0, 0, 31);
    glClearPolyID(63);

    glClearDepth(0x7FFF);

    glViewport(0, 0, 255, 191);

    // Setup some VRAM as memory for textures
    vramSetBankA(VRAM_A_TEXTURE);

    // Load texture
    glGenTextures(1, &textureID);
    glBindTexture(0, textureID);
    glTexImage2D(0, 0, GL_RGB, TEXTURE_SIZE_128 , TEXTURE_SIZE_128, 0,
                 TEXGEN_TEXCOORD, (u8*)neonBitmap);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70, 256.0 / 192.0, 0.1, 40);

    gluLookAt(0.0, 0.0, 2.0,  // Position
              0.0, 0.0, 0.0,  // Look at
              0.0, 1.0, 0.0); // Up

    // Setup sub screen for the text console
    consoleDemoInit();

    // Setup sound bank
    mmInitDefaultMem((mm_addr)soundbank_bin);

    // load the module
    mmLoad(MOD_JOINT_PEOPLE);

    // load sound effects
    mmLoadEffect(SFX_FIRE_EXPLOSION);

    // Start playing module
    mmStart(MOD_JOINT_PEOPLE, MM_PLAY_LOOP);

    int angle_x = 0;
    int angle_z = 0;

    while (1)
    {
        // Synchronize game loop to the screen refresh
        swiWaitForVBlank();

        // Print some text in the demo console
        // -----------------------------------

        // Clear console
        printf("\x1b[2J");

        // Print current time
        char str[100];
        time_t t = time(NULL);
        struct tm *tmp = localtime(&t);
        if (strftime(str, sizeof(str), "%Y-%m-%dT%H:%M:%S%z", tmp) == 0)
            snprintf(str, sizeof(str), "Failed to get time");
        printf("%s\n\n", str);

        // Print contents of the BIN file
        for (int i = 0; i < data_string_bin_size; i++)
            printf("%c", data_string_bin[i]);
        printf("\n");

        // Print some controls
        printf("PAD:    Rotate triangle\n");
        printf("START:  Exit to loader\n");
        printf("A:      Play SFX\n");
        printf("\n");

        // Test code from a different folder
        const char *name = "A name";
        printf("Name: [%s]\n", name);
        printf("Name lenght: %d\n", my_strlen(name));

        // Handle user input
        // -----------------

        scanKeys();

        uint16_t keys = keysHeld();
        uint16_t keys_down = keysDown();

        if (keys & KEY_LEFT)
            angle_z += 3;
        if (keys & KEY_RIGHT)
            angle_z -= 3;

        if (keys & KEY_UP)
            angle_x += 3;
        if (keys & KEY_DOWN)
            angle_x -= 3;

        if (keys_down & KEY_A)
            mmEffect(SFX_FIRE_EXPLOSION);

        if (keys & KEY_START)
            break;

        // Render 3D scene
        // ---------------

        glPushMatrix();

        glRotateZ(angle_z);
        glRotateX(angle_x);

        glMatrixMode(GL_MODELVIEW);

        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE);

        glBindTexture(0, textureID);

        glColor3f(1, 1, 1);

        glBegin(GL_QUADS);

            GFX_TEX_COORD = (TEXTURE_PACK(0, inttot16(128)));
            glVertex3v16(floattov16(-1), floattov16(-1), 0);

            GFX_TEX_COORD = (TEXTURE_PACK(inttot16(128),inttot16(128)));
            glVertex3v16(floattov16(1), floattov16(-1), 0);

            GFX_TEX_COORD = (TEXTURE_PACK(inttot16(128), 0));
            glVertex3v16(floattov16(1), floattov16(1), 0);

            GFX_TEX_COORD = (TEXTURE_PACK(0,0));
            glVertex3v16(floattov16(-1), floattov16(1), 0);

        glEnd();


        glPopMatrix(1);

        glFlush(0);
    }

    return 0;
}
