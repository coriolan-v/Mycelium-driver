# Mushroom firmware v3.0

Teensy 4.1 Art-Net receiver for **16 parallel outputs, 3 universes per output**.
Each universe carries 170 RGB pixels (510 channels). Each output therefore
drives 510 LEDs: 8,160 LEDs and 48 universes in total.

## Build and upload

Open this `Firmware` folder in VS Code with PlatformIO IDE. From a PlatformIO
terminal:

```sh
pio run
pio run --target upload
```

The default build uses `Mushroom_FW_3.0/main.cpp`, the local OctoWS2811 library, and pinned
QNEthernet 0.37.0. PlatformIO installs dependencies on the first build. The old
Arduino sketches and `_dev` experiments remain as references and are excluded
from the build. NativeEthernet, FNET, and the old Art-Net library are excluded.
Connect the Teensy over USB to upload. If automatic reset fails, press its
program button when prompted.

The firmware version is `3.0.0`. The upload image is generated at
`.pio/build/teensy41/Mushroom_FW_3.0.hex`.

## Sender configuration and wiring

Set the sender to unicast ArtDmx to **10.0.0.9:6454**, with RGB channel order
and 510 channels per universe. The receiver accepts 512-channel packets too,
but ignores channels 511 and 512; pixels never span universe boundaries.

Configuration is in `include/config.h`. The subnet is `255.0.0.0`; no gateway
or DHCP is needed for a local lighting network. The board uses its unique
factory MAC. Set a unique static IP for each controller.

| Output | Teensy pin | Art-Net wire universes | Pixels |
| --- | --- | --- | --- |
| 1 | 0 | 0–2 | 510 |
| 2 | 1 | 3–5 | 510 |
| 3 | 2 | 6–8 | 510 |
| 4 | 3 | 9–11 | 510 |
| 5 | 4 | 12–14 | 510 |
| 6 | 5 | 15–17 | 510 |
| 7 | 6 | 18–20 | 510 |
| 8 | 7 | 21–23 | 510 |
| 9 | 8 | 24–26 | 510 |
| 10 | 9 | 27–29 | 510 |
| 11 | 10 | 30–32 | 510 |
| 12 | 11 | 33–35 | 510 |
| 13 | 12 | 36–38 | 510 |
| 14 | 24 | 39–41 | 510 |
| 15 | 25 | 42–44 | 510 |
| 16 | 26 | 45–47 | 510 |

These pins follow the first 16 entries of `_dev/Mushroom_FW_2`. Adjust `kPins`
to match the actual controller wiring. Sender interfaces that label universes
starting at 1 may display these as 1–48; the addresses on the wire are 0–47.
Connect the Teensy native Ethernet interface and use suitable LED data level
shifting and a shared ground. LED output is GRB at 800 kHz.

## Performance and frame behaviour

- 600 MHz CPU, `-O3` and link-time optimisation; no overclock required.
- Direct lwIP UDP receive callbacks avoid an Arduino UDP packet queue and
  additional application packet copies for contiguous packets.
- Incoming RGB is converted directly into the drawing buffer. There is no
  per-pixel API dispatch or second full-frame colour conversion.
- OctoWS2811 drives all 16 pins simultaneously using DMA while the main loop
  continues receiving. Separate drawing/display buffers prevent DMA tearing.
- No application `delay()`, USB wait, startup animation, or per-packet logging.
  `show()` is called only after DMA and the reset interval finish. The local
  driver fixes a `busy()` bug that previously ignored active DMA.
- A complete set of 48 universes is presented as soon as the output is free.
  If packets are missing, a non-blocking 15.6 ms deadline from the first pending
  packet allows an incomplete update; missing universes retain their last
  values. Short universes clear their remaining pixels.
- While output is busy, incoming data replaces pending values instead of
  building a frame queue. Above the LED refresh rate, intermediate frames are
  discarded and a snapshot can contain the latest values from different source
  frames. This favours latency over preserving every source frame.
- ArtDmx sequence checks operate independently per universe, handle 255-to-1
  wrap, and reset after one second of inactivity. Sequence 0 disables ordering.
  Use one sender per receiver; source merging is not implemented.

At 800 kHz, 510 RGB pixels take 15.3 ms to transmit. The driver adds a 300 µs
reset interval, giving a **theoretical ceiling of about 64 FPS** before CPU and
network overhead. This is a calculated limit, not a measured hardware result.
Changing CPU speed cannot remove the LED wire time. Prefer unicast and a wired
network. Configure the sender for up to 60 FPS initially.

This is a directly addressed ArtDmx receiver. ArtPoll discovery, ArtSync frame
locking, RDM, and multiple-source merging are not implemented. It holds the
last output if input stops. `receiver.stats` exposes counters to a debugger
without adding serial traffic to the receive loop.

## Verification

`pio run` compiles the Teensy firmware. Portable receiver tests compile and
execute the same parser, mapping, sequencing, and frame-deadline code on a PC:

```sh
python tests/run_tests.py
```

The test runner uses `g++`, `clang++`, or the compiler selected by `CXX`.
On Windows without a host compiler, an optional project-local installation is:

```sh
mkdir .pio/test-tools
pio pkg install --global --storage-dir .pio/test-tools --tool platformio/toolchain-gccmingw32
```

Hardware validation still requires checking colour order and all 16 outputs,
then streaming all 48 universes at the desired frame rate while checking packet
loss, output timing, and Ethernet reconnect behaviour.

QNEthernet is distributed under AGPL-3.0-or-later; its license and dependencies
are included in the installed package.
