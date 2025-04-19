# Sprite Demo

Creating sprites is rather easy. All you have to do is:

- Design a sprite
- Define the colors
- Activate the sprite
- Move it
- Release resources

[Amiga Sprites](http://amigadev.elowar.com/read/ADCD_2.1/Hardware_Manual_guide/node00AE.html)

## Design sprite

Sprites are 16 pixels wide and have no limitation in height.

You can use, e.g., [Online Sprite](https://amigafonteditor.ozzyboshi.com/index_sprite.html) to define the shape.

## Define the colors

You can create up to 8 sprites. Each can use 4 colors, where one color represents transparency. This results in finally 3 colors. The used colors are fixed.

| Sprite number  | colors  |
| -------------- | ------- |
| 0, 1           | 17-19   |
| 2, 3           | 21-23   |
| 4, 5           | 25-27   |
| 6, 7           | 29-31   |

## Activate the sprite

```c
struct SimpleSprite sprite = {0};
// reserve sprint number 2
sprite_num = GetSprite(&sprite, 2);
// initial position
sprite.x = 1;
sprite.y = 1;
// define height. required
sprite.height = SPRITE_HEIGHT;
// initialize sprite
ChangeSprite(NULL, &sprite, (APTR)spriteData);
```

## Move it

```c
sprite.x = new_position_x;
sprite.y = new_position_y;
// Move sprite to new position
MoveSprite(NULL, &sprite, sprite.x, sprite.y);
// Wait for next frame
WaitTOF();
```

## Release resources

```c
FreeSprite((WORD)sprite_num);
```

# Compile

As compiler SAS/C 6.51 is used

```cmd
sc link sprite.c
```
