# Mushroom firmware v3.0

Each Teensy 4.1 is an independent Art-Net/ArtSync receiver with its own IP and
universe range. There is one PlatformIO environment, `teensy41`, and no driver
numbering or automatic address offsets.

## Set the IP

Edit the settings at the top of `include/config.h` before uploading each board:

```cpp
constexpr uint8_t kIP[] = {10, 0, 0, 9};
constexpr uint8_t kNetmask[] = {255, 0, 0, 0};
constexpr uint8_t kGateway[] = {0, 0, 0, 0};
constexpr uint16_t kStartUniverse = 0;
constexpr size_t kActivePorts = 16;
```

For two controllers, use unique IPs, for example `10.0.0.9` and `10.0.0.10`.
Both may use universes **0-47**: Unity sends different data to each board's IP
using unicast on UDP port 6454. Change `kStartUniverse` only if you want a
different range on that controller. The board uses its unique factory MAC.
Configure the Unity computer's lighting interface on the same subnet.

## Build, upload, and see FPS

Open this `Firmware` folder in VS Code with PlatformIO IDE. Connect the board
whose IP you just configured, then run:

```sh
pio run --target upload
pio device monitor -b 115200
```

Change `kIP` before building and uploading to the other board. If multiple boards
are connected, use `--upload-port COMx` for upload and `--port COMx` for the monitor.
Press the program button if automatic reset fails. `pio run` builds without uploading.

The image is `.pio/build/teensy41/Mushroom_FW_3.0.hex`. PlatformIO installs the
pinned Teensy platform and QNEthernet dependency on the first build. Sources
are in `Mushroom_FW_3.0/main.cpp`; older sketches are excluded from the build.

## Port layout and capacity

Each rod has 36 RGB pixels. Ten rods per output means **360 pixels per output**,
using three ArtDmx universes: **510 + 510 + 60 channels**. The third universe
contains the last 20 pixels. Padded 512-channel packets are accepted; padding
cannot spill into the next output. Input and TM1812 wire order are RGB. This
exchanges red and green relative to the previous firmware's GRB output, as
requested; blue is unchanged. Unity should continue sending RGB. The receiver
copies pixel bytes directly, with no per-pixel colour conversion.

The PlatformIO build enables `MUSHROOM_TM1812=1` in the local OctoWS2811 driver:
nominal 400 ns zero-high pulses, 850 ns one-high pulses, and 800 kHz signalling.
The existing 300 us reset interval is retained, covering the 200 us minimum in
the [TM1812 V1.6 datasheet](https://www.lumikit.com.br/firmware/hslc-tm/datasheets/TM1812_V1.6.pdf).
The driver transmits the prepared bytes without an additional colour swap.
Use a suitable 5 V logic buffer; the datasheet specifies a 3.8 V minimum high
input. Verify colours and waveform timings on the actual rods.

| Teensy pin | Universes with start universe 0 |
| --- | --- |
| 0 | 0-2 |
| 1 | 3-5 |
| 2 | 6-8 |
| 3 | 9-11 |
| 4 | 12-14 |
| 5 | 15-17 |
| 6 | 18-20 |
| 7 | 21-23 |
| 8 | 24-26 |
| 9 | 27-29 |
| 10 | 30-32 |
| 11 | 33-35 |
| 12 | 36-38 |
| 24 | 39-41 |
| 25 | 42-44 |
| 26 | 45-47 |

All 16 ports are enabled. Each board supports **160 rods / 5,760 pixels** at
these settings, or **320 rods / 11,520 pixels across two boards**. This layout
does not accommodate the earlier 600-rod total; that would need a different
rod distribution, strip length, and universe allocation or more controllers.

Change `kPins` to match your wiring. Physical pin numbers need not be consecutive.
`kActivePorts` enables the first N entries. Some sender interfaces display wire
universe 0 as universe 1. Use native Ethernet, suitable LED data level shifting,
and a shared ground.

## ArtSync with Unity

For a coordinated lighting frame, unicast each controller's universe data to
its own IP, then broadcast one ArtSync on UDP port 6454. With the configured
/8 subnet, use `10.255.255.255`. Both controllers retain their independent
content and addresses; matching broadcast ArtSync coordinates their release.
There is no master/slave relationship between controllers.

Using an existing C# `System.Net.Sockets.UdpClient` sender:

```csharp
byte[] artSync = {
    0x41, 0x72, 0x74, 0x2D, 0x4E, 0x65, 0x74, 0x00,
    0x00, 0x52, // OpSync, little endian
    0x00, 0x0E, // protocol version 14
    0x00, 0x00
};
udp.EnableBroadcast = true;
// Reuse the ArtDmx socket, bound to the lighting network interface.
// Send once after all the current frame's ArtDmx packets.
udp.Send(artSync, artSync.Length,
    new System.Net.IPEndPoint(System.Net.IPAddress.Parse("10.255.255.255"), 6454));
```

ArtSync must come from the same source IP as the controller's current ArtDmx
sender. Each board starts asynchronously and enters sync mode on its first
matching ArtSync. Released frames are frozen separately from arriving data.
If DMA is busy, the snapshot waits without blocking reception. A newer released
snapshot replaces an unsent one instead of building a latency queue.

While synchronized, data is released only on matching ArtSync. After four
seconds without sync, each controller independently returns to asynchronous
operation. In that mode, a complete set of universes releases when output is
free; partial updates have a non-blocking 11.1 ms deadline. Missing universes
retain their last values; short universes clear unused pixels. Empty syncs
do not repeatedly transmit unchanged output. Use one sender per controller;
source merging is not implemented.

Source matching and fallback follow the
[Art-Net specification](https://art-net.org.uk/downloads/art-net.pdf). ArtSync
coordinates release, not hardware clocks: network timing and busy outputs can
still introduce differences. Broadcasts must reach the lighting LAN.

## FPS report

The USB monitor receives a report once per second. Example:

```text
fps=60.0 mode=SYNC frames=600 partial=0 sync=600 replaced=0 bad=0 old=0
```

This is example output, not a hardware benchmark.

- `fps`: completed LED transfers per second over the actual elapsed interval,
  counted after DMA and the reset interval finish. It is not Unity FPS or packet rate.
- `mode`: `SYNC` or `ASYNC`.
- `frames`: cumulative completed network-driven frames, excluding startup black.
- `partial`: cumulative submitted frames missing expected universes.
- `sync`: cumulative matching ArtSync packets.
- `replaced`: snapshots replaced or cancelled before output could start.
- `bad`: malformed, unsupported, or unmapped packets; not a network-loss counter.
- `old`: duplicate or out-of-order ArtDmx packets rejected by sequence checks.

Reporting does not wait for USB connection and skips reports when the transmit
queue lacks room. There is no per-frame or per-packet serial logging. For an
independent physical check, measure intervals between LED data bursts using a
logic analyser: `FPS = 1,000,000 / interval_us`. Each burst lasts about 10.8 ms.

## Performance and verification

600 MHz CPU, `-O3`, link-time optimisation, direct UDP callbacks, and parallel
DMA output. Separate staging, released-frame, and display buffers prevent
incoming packets from changing output in progress. No application delays,
USB connection wait, or startup animation. The bundled LED driver fixes its
DMA-busy check. Sequence checks handle wrap and reset after sender inactivity.

360 RGB pixels take 10.8 ms at 800 kHz plus the driver's 300 us reset interval:
about **90 FPS theoretical maximum**, before software and network overhead.
Start Unity at 60 FPS and measure on hardware. Pending frames may be replaced
if the sender exceeds the output rate.

Run portable tests for the currently configured controller with:

```sh
python tests/run_tests.py
```

Tests cover mapping, final-universe clipping, malformed packets, sequence and
clock wrap, ArtSync source filtering/timeout, frozen snapshots, and FPS timing.
The runner uses `g++`, `clang++`, or the compiler in `CXX`. An optional Windows
compiler can be installed locally:

```sh
mkdir .pio/test-tools
pio pkg install --global --storage-dir .pio/test-tools --tool platformio/toolchain-gccmingw32
```

Hardware validation still requires checking colours, all outputs, FPS, dropped
sync, and Ethernet reconnect. ArtPoll discovery and RDM are not implemented.
QNEthernet is AGPL-3.0-or-later; its license is in the installed package.
