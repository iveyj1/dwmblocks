# dwmblocks

Personal modular status bar for dwm.

## Build and install

```sh
make clean
rm -f blocks.h
make
sudo make install
```

Start it before dwm, for example in `.xinitrc`:

```sh
dwmblocks &
exec dwm
```

## Mint / Void integration

No distro or shared session helper is required. Build with a C compiler, make,
and Xlib headers (`libx11-dev` on Mint, `libX11-devel` on Void). Status commands
use procps tools, `wpctl` (PipeWire/WirePlumber), and `iw`; Wi-Fi reporting does
not depend on a particular network manager. The battery helper currently
expects BAT0 with current_now/voltage_now; other hardware may need adjustment.

The optional separate `configure-sl` session launcher starts and cleans up
this process when installed. dwm itself remains usable without dwmblocks.
Do not also start it in .xprofile when using that launcher.

## Configuration

Edit `blocks.def.h`, remove generated `blocks.h`, then rebuild. `blocks.h` is copied from `blocks.def.h` when missing. This repo tracks both, so keep them synchronized.

Current local blocks include:

- Date/time
- PipeWire volume/mute status via `wpctl`
- Wi-Fi status via `dwmblocks-wifi`
- CPU usage
- Memory usage
- Battery charge/discharge rate via `dwmblocks-charge-rate`

Each block has:

```c
{"icon", "shell command", interval_seconds, signal_number}
```

Output is limited to 255 bytes in total, matching dwm's status buffer. Valid
UTF-8 is truncated only at character boundaries. Empty command output suppresses
the entire block (including its icon); separators appear only between displayed
blocks. Icons are prefixes, not standalone blocks. Only the first output line is
shown; remaining output is drained before waiting for the command to exit.
Commands must terminate; this does not impose a backend timeout.

`delim` sets the default separator; no separate `delimLen` is needed. Use
`dwmblocks -d ' / '` to override it or `-d ''` for none. `dwmblocks -p` writes to
stdout without opening an X display. Unknown options or a missing `-d` argument
are errors.

A nonzero interval refreshes automatically. A nonzero signal can be refreshed with a realtime signal using the dwmblocks signal patch behavior.

## Local changes

- Linux status defaults usable on Void and Mint.
- Wi-Fi helper script.
- Battery charge-rate helper script.
- Build/install support for local helper scripts.
- Retains signal-update support from dwmblocks patch lineage.
