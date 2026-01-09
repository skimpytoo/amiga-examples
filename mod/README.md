# MOD Player Example

Simple example that plays ProTracker MOD files using ptplayer on Amiga.

## Requirements

- SDI-Headers: <http://aminet.net/package/dev/c/SDI_headers>
- ptplayer: <http://aminet.net/package/mus/play/ptplayer>
- vasmm68k_mot assembler: <http://sun.hasenbraten.de/vasm/index.php?view=binrel>
- SAS/C toolchain (sc and link) or a compatible Amiga C compiler
- AmigaOS with 68020+ CPU

## Generate ptplayer.o

1. Download vasmm68k_mot from the vasm releases and copy it to C: on the Amiga.
2. Assemble ptplayer.asm:

   ```cmd
   vasmm68k_mot -Fhunk -m68020 -o ptplayer.o ptplayer.asm
   ```

   Note: `-m68020` is required because ptplayer.asm needs ≥ 68010.

## Compile

Ensure SDI-Headers are installed and ptplayer.o is available under `ptplayer/`.

```
sc DATA=FAR link playMod.c ptplayer/ptplayer.o
```

Adjust paths as needed for your setup.
