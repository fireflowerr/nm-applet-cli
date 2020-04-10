# nm-applet-cli
 A [fork](https://gitlab.gnome.org/GNOME/network-manager-applet) of GNOME's nm-applet that allows for controlling the popup menu via a command line interface.
 
### patch usage: nm-applet ...
* --help - prints helpful information
* --menu - triggers the nm-applet left-click menu
* --status - triggers the nm-applet right-click menu
* --set-pid [pid] - manually sets the pidfile to [pid]
 
### build instructions:
```bash
» meson . build -D selinux=false
» ninja -C build
```
