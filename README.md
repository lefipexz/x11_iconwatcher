If you ever wanted to change the _NET_WM_ICON of an window in X11 this is what you are looking for.
This is an event based program write in C that changes the _NET_WM_ICON to any image that you upload and direct in the scrpit.

The program is made with the intention to help with this problem, for those who wish to still in X11, as it doesnt happen on Wayland.

Its my obligation to remember that this script is not finished and its 100% "vibecoded", any forks, solutions and newer versions are welcome



Talking about requirements, this is the only library you need, its the libx11-dev, you can install it like this:
sudo apt install build-essential libx11-dev

To build the script, you can use this comand below:
gcc script/iconwatcher2.c -o iconwatcher2 -lX11

To config and edit the watcher you should convert your image to the appropriate
format, copy the config before it, change the window name, change the path to
your image and recompile

Usage is simple, on terminal:
./iconwatcher2
To autostart it, configure it on your distro session launcher
