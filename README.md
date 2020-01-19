This is the KeyWalker IFD-68 firmware burner for Linux OS which was
reversed from the original Windows firmware.

Directory structure:

* **original/** -- original firmware and documentation

* **reverse/** -- default.bin and default.keymap.bin are raw dumps,
    parse.py is a simple Python script which converts raw dump
    (default.keymap.bin) to the ascii key map (default.keymap).

* **burner/** -- this is a firmware burner

Instructions to burn a firmware:

0. Go to the burner/ directory and run `make` to compile the ifd68 binary.

1. At the first time run the `./ifd68` without any arguments. It will
generate a default map.asc file, a map of keys for keyboard. Each line
of a file is a record. Each record consists of three columns separated
by a whitespace. The first column is a FN_MOD (FN_OFF - FN key
released, FN_ON - FN key pressed). The second column is a key on the
keyboard for its default position. The third column represents the key
which one wants to bind to this position.

2. Edit the map.asc file. For example, to swap '-' and '+' one need to edit 12 and 13 lines like this:
```shell
FN_OFF - +
FN_OFF + -
```

3. To write a changes to the keyboard just place map.asc file name to the first argument of ifd68:
```shell
./ifd68 map.asc
```

Caution: when one want to burn a new firmware he/she need to do it from the other keyboard or reboot (reconnect the USB cord) the keyboard after burning.

Any help would be great!

Contact: coffe92@gmail.com (Nikita)
