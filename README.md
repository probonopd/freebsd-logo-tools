# FreeBSD Boot Logo Converter

Allows to edit the logo in [`sys/dev/vt/logo/logo_freebsd.c`](https://raw.githubusercontent.com/freebsd/freebsd-src/main/sys/dev/vt/logo/logo_freebsd.c) by converting it from and to a bitmap image that can be edited with any image editor.

## Building

```sh
clang logo2bmp.c -o logo2bmp
clang bmp2logo.c -o bmp2logo
```

## Usage

```sh
fetch "https://raw.githubusercontent.com/freebsd/freebsd-src/main/sys/dev/vt/logo/logo_freebsd.c"
./logo2bmp
# edit logo.bmp
./bmp2logo
diff logo_freebsd.c logo_modified.c # No difference if logo.bmp was not modified
```

## Credits

This software was developed by Simon Peter based on research by Jesper Schmitz Mouridsen.
