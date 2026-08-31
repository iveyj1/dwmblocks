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

A nonzero interval refreshes automatically. A nonzero signal can be refreshed with a realtime signal using the dwmblocks signal patch behavior.

## Local changes

- Void-oriented status defaults.
- Wi-Fi helper script.
- Battery charge-rate helper script.
- Build/install support for local helper scripts.
- Retains signal-update support from dwmblocks patch lineage.
