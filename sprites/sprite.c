#include <exec/types.h>
#include <graphics/gfx.h>
#include <graphics/gfxbase.h>
#include <graphics/gfxmacros.h>
#include <graphics/sprite.h>
#include <intuition/intuitionbase.h>
#include <intuition/screens.h>
#include <hardware/custom.h>
#include <hardware/dmabits.h>
#include <libraries/dos.h>

#include <clib/graphics_protos.h>
#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>
#include <clib/alib_stdio_protos.h>
#include <clib/lowlevel_protos.h>

#include <stdlib.h>

struct GfxBase* GfxBase = NULL;
struct IntuitionBase* IntuitionBase = NULL;
extern struct Custom far custom;

#define SPRITE_HEIGHT 7  // Number of lines of sprite (see spriteData )
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

// simple sprite data. http://amigadev.elowar.com/read/ADCD_2.1/Hardware_Manual_guide/node00BC.html
// Needs to be in chip memory
// use e.g. https://amigafonteditor.ozzyboshi.com/index_sprite.html
UWORD chip spriteData[] = {
    0, 0,            /* position control           */
    0x3800, 0x0000,  // line 1
    0x4400, 0x2800,  // line 2
    0x8200, 0x4400,  // line 3
    0x9200, 0x1000,  // line 4
    0x8200, 0x4400,  // line 5
    0x4400, 0x2800,  // line 6
    0x3800, 0x0000,  // line 7
    0, 0             // reserved, must init to 0 0
};

VOID main(int argc, char** argv) {
    struct SimpleSprite sprite = {0};

    WORD sprite_num;
    SHORT move_x = 1, move_y = -1;
    struct Screen* screen;
    int return_code;

    return_code = RETURN_OK;

    if (NULL == (GfxBase = (struct GfxBase*)OpenLibrary("graphics.library", 37L))) {
        return_code = RETURN_FAIL;
    } else {
        if (NULL == (IntuitionBase = (struct IntuitionBase*)OpenLibrary("intuition.library", 37L))) {
            return_code = RETURN_FAIL;
        } else {
            if (NULL == (screen = OpenScreenTags(NULL,
                                                 SA_Left, 0,
                                                 SA_Top, 0,
                                                 SA_Width, SCREEN_WIDTH,
                                                 SA_Height, SCREEN_HEIGHT,
                                                 SA_Depth, 6,
                                                 SA_Title, NULL,
                                                 SA_Quiet, TRUE,
                                                 SA_ShowTitle, FALSE,
                                                 TAG_DONE))) {
                return_code = RETURN_FAIL;
            } else {
                if (-1 == (sprite_num = GetSprite(&sprite, 2))) {
                    return_code = RETURN_WARN;
                } else {
                    // Define some colors for the sprite
                    SetRGB4(&screen->ViewPort, 21, 12, 3, 8);
                    SetRGB4(&screen->ViewPort, 22, 13, 13, 13);
                    SetRGB4(&screen->ViewPort, 23, 4, 4, 15);

                    // initial position
                    sprite.x = 1;
                    sprite.y = 1;
                    // define height. required
                    sprite.height = SPRITE_HEIGHT;

                    // initialize sprite
                    ChangeSprite(NULL, &sprite, (APTR)spriteData);

                    while (GetKey() != 69) {  // ESC to terminate
                        // Bounce at border
                        if (sprite.x == SCREEN_WIDTH - 8 || sprite.x == 0) {
                            move_x = move_x * -1;
                        }
                        if (sprite.y == SCREEN_HEIGHT - SPRITE_HEIGHT || sprite.y == 0) {
                            move_y = move_y * -1;
                        }
                        sprite.x = sprite.x + move_x;
                        sprite.y = sprite.y + move_y;
                        // Move sprite to new position
                        MoveSprite(NULL, &sprite, sprite.x, sprite.y);
                        // Wait for next frame
                        WaitTOF();
                    }
                    FreeSprite((WORD)sprite_num);
                }
                CloseScreen(screen);
            }
            CloseLibrary((struct Library*)IntuitionBase);
        }
        CloseLibrary((struct Library*)GfxBase);
    }
    exit(return_code);
}