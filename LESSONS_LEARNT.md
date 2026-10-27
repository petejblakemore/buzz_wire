# Lessons learnt: Buzz Wire (October 2026)

Built and working in one evening, and the sketch compiled on its first try. The habits from the Restless Coffin paid off: confirming the hardware first, one netlist with a connection check, a hole-by-hole wiring diagram, and power-up checks in order. Every snag was physical or a setup step, not the design.

## What went right
- **Hardware confirmed before designing:** a photo of the ESP32, the pin-row spacing measured (25 mm, so 9 empty holes between the rows), the sound module's pin order read off the board, and which end its USB socket is at.
- **The board matched the module pins.** Several ESP32 pins sat on the same strip as a sound-module trigger pin, so those signals needed no links.
- **5V only:** no 12V supply, no buck converter, no MOSFET, which left out the parts that caused most of the coffin's faults.
- **Tests in stages:** USB-only bench test with a jumper wire first, then sound on 5V power.

## Snags and fixes
| Snag | Cause | Fix / lesson |
|---|---|---|
| C1 blocked the sound module's USB socket | Module soldered in, not socketed; layout didn't check socket access | Moved C1 to X21/W21. **Mark keep-clear zones** for USB sockets and buttons in every layout |
| "Disk not readable" on the sound module | Blank flash memory | Disk Utility: **MS-DOS (FAT), Master Boot Record** |
| Upload failed after "Changing baud rate to 921600" | USB link too slow for that speed | **Upload Speed 115200** on this board |
| `.fseventsd` on the module's drive | macOS hidden folder | Harmless. `dot_clean` removes the `._` files that do matter |
| WAV files to convert | The Mac can't make MP3s natively | `brew install ffmpeg`. Mono, 44.1 kHz, 128k, silence trimmed, normalised |
| Sources disagreed on BUSY polarity | Datasheets and write-ups conflict | Left BUSY unconnected. **Don't design around a pin whose behaviour sources disagree on** |
| 3 × AA asked about | 3.6–4.8V is too low for the ESP32 regulator, and sags on the amplifier's surges | 5V adapter (5.23V is fine) or a USB power bank |
| Capacitor values (330µF, 10nF) | Substitutes from the parts box | Both fine. What matters is the voltage rating and the electrolytic's polarity |

## New rules for Weyoun
1. **Ask "socketed or soldered?" for every module.** If soldered, plan how its USB socket is reached and keep tall parts away from it.
2. **Check physical access** for USB sockets, BOOT/EN buttons, the jumper and the speaker wires. List the keep-clear holes on the layout.
3. **Default the ESP32 upload speed to 115200** in the instructions.
4. **Include module setup in the pack:** formatting, file names and order, `dot_clean`, and which cables must be unplugged first.
5. **Flag pins that sources disagree on,** and design so they aren't needed.
6. **When the pin plan changes between suggestion and design, say so clearly.**

## Prompt to resume or extend
> Weyoun, read LESSONS_LEARNT.md and README.md in ~/Projects/buzzwire before we start. Buzz Wire v1 works. I want to [change / add]. Keep the board as built unless I say otherwise, and tell me exactly which holes change. Update the netlist, rerun the connection check, and list any new keep-clear zones.
