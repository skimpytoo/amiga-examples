# Load and display iff/ilbm images

Example about loading and displaying an IFF image with iff.library.
The example is an adapted code of EasyExample provided in the archive.

## Requirements

Download [iff.library](https://aminet.net/package/util/libs/iff.library-23) by Christian A. Weber

- Extract the LHA file.
- Put iff.library into your system's LIBS folder (required to execute).
- Copy iff.h here into the 'includes' subfolder (required for compilation).

## Example

With the help of the iff.library, we load the image provided in the asset folder and display it on a custom screen.
As the library is straightforward to use, there is not much more to explain. ;)

## Compile

As compiler SAS/C 6.51 is used

```cmd
sc link displayIff.c
```
