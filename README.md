# Buzz Wire (Halloween 2026)

Guide the wand from the start pad to the finish pad without touching the wire. You have 3 lives: each touch boos and puts out a red LED, losing all 3 brings an evil laugh, and reaching the finish brings a cheer. While it's waiting, it whispers "come and play" every 1–3 minutes.

**Status: v1 working** (built and tested 27 Oct 2026).

## Hardware
- ESP32 DevKit V1 (30-pin, pin rows 25 mm apart), in female headers in columns 7 and 17
- DY-SV17F sound module (4 MB flash), soldered in columns 25 and 33, in I/O Independent mode 0
- Stripboard 24 strips (A–X) × 37 holes, strips lengthways
- 5V 2A adapter (5.23V measured) to X37 (+) and W37 (−)
- 5 LEDs (green, amber, 3 red) with 330Ω resistors; 4–8Ω speaker

## Files
| Path | What |
|---|---|
| `buzz_wire/buzz_wire.ino` | Sketch. Board: DOIT ESP32 DEVKIT V1, **Upload Speed 115200** |
| `hardware/netlist.txt` | Every connection on the board, by hole; passes the connection check |
| `LESSONS_LEARNT.md` | What went wrong and right, and prompts for next time |

## Pins
| ESP32 | Does |
|---|---|
| D32 / D33 / D27 | Course wire / start pad / finish pad (wand is GND) |
| D23 / D22 | Green / amber LED |
| D21 / D25 / D26 | Lives 1–3 (red LEDs) |
| D19 / D18 / D17 / D16 | Sound module IO0 boo / IO1 cheer / IO3 laugh / IO4 come and play |

## Sounds
Files `00001.mp3` boo, `00002.mp3` cheer, `00003.mp3` filler (copy of the boo), `00004.mp3` laugh, `00005.mp3` come and play. Use mono 44.1 kHz 128 kbps, copied in number order, then run `dot_clean`.

To change sounds: unplug the 5V adapter, the ESP32 USB and JP1, then plug the module's own USB into the Mac. Never connect the module's USB and the 5V adapter at the same time.

## Programming
JP1 off, 5V adapter off, ESP32 on USB. Upload at 115200 (it fails at 921600).
