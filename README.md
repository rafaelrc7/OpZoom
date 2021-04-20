# OpZoom

This is a simple program, supposed to be compatible with both Windows and Linux, with the main purpose of easily opening Zoom with the right meeting according to the time it is ran.

It can be compiled with Lua support, so you can edit a lua file with your meetings info without the necessity of re-compiling the project for it to take effect. However you can also compile it without lua and keep your meetings in an array inside the program source.
## Building

With lua support:

```sh
gcc -O2 -o opzoom ozoom.c -llua
```
Withou lua support:

```sh
gcc -O2 -o opzoom ozoom.c -D _NO_LUA
```

## Example meeting configuration

### With lua support, create a zoom_classes.lua like this:

```lua
CLASSES = {
		{discipline_code="FOO1234", meeting_id="00000000000", meeting_pass="xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", days = (MONDAY | WEDNESDAY),	time = 11},
		{discipline_code="BAR5678", meeting_id="00000000000", meeting_pass="xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", days = (FRIDAY),				time = 11},
}
```

### Without lua support:

Check example inside the source file.

## License
[MIT](https://choosealicense.com/licenses/mit/)

## Third Party License
[Lua (MIT)](https://www.lua.org/license.html)
