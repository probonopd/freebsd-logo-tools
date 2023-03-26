# FreeBSD Boot Logo Tools

`logo2bmp` allows to edit the FreeBSD boot logo in [`sys/dev/vt/logo/logo_freebsd.c`](https://raw.githubusercontent.com/freebsd/freebsd-src/main/sys/dev/vt/logo/logo_freebsd.c) by converting it to a bitmap image that can be edited with any image editor. `bmp2logo` converts the bitmap back to C source code.

## Building

```sh
make
```

## Usage

```sh
fetch "https://raw.githubusercontent.com/freebsd/freebsd-src/main/sys/dev/vt/logo/logo_freebsd.c"
./logo2bmp
# edit logo.bmp
./bmp2logo
diff logo_freebsd.c logo_modified.c # No difference if logo.bmp was not modified
./logo2kernel
# Copy kernel to /boot/kernel/kernel.modified
sudo cp kernel.modified /boot/kernel/
```

To boot once manually with the modified logo without changing the default boot loader entry, you can use the following command at the FreeBSD boot loader prompt:

```
unload
set bootfile="kernel.modified"
boot-conf
```

In helloSystem, you can get to the FreeBSD boot loader prompt by pressing the Backspace key as soon as the screen turns gray, and then pressing the escape key.

Once you are satisfied with the modified kernel, you can make it the default by running the following command:

```sh
sudo cp /boot/kernel/kernel /boot/kernel/kernel.original
sudo mv /boot/kernel/kernel.modified /boot/kernel/kernel
```

## Notes

It seems like the raw binary image is stored in `/boot/kernel/kernel` at an offset following the hex pattern `01 01 00 00  DB 00 00 00  01 00 00 00  00 00 00 00` (to be verified whether this is always the case). We can find the start of this hex pattern using

```sh
% binwalk -R  "\x01\x01\x00\x00\xDB\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00" /boot/kernel/kernel

DECIMAL       HEXADECIMAL     DESCRIPTION
--------------------------------------------------------------------------------
24009232      0x16E5A10       \x01\x01\x00\x00\xDB\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00
```

## Credits

This software was developed by Simon Peter based on research by Jesper Schmitz Mouridsen.
