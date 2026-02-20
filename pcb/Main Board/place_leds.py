"""
place_leds.py — KiCad PCB Editor Scripting Console script

Positions each SK6812-E (IC*) at a fixed offset from its paired
Kailh Choc V1 switch (SW*). Re-run any time you move the switches.

Usage:
  1. Open your .kicad_pcb in PCB Editor
  2. Tools → Scripting Console
  3. Paste this entire script and press Enter
  4. Ctrl+Z to undo if the offset needs tweaking

The LED will also inherit the switch's rotation so the offset
stays correct regardless of switch orientation.
"""

import pcbnew
import math

# ──────────────────────────────────────────────────────────────
# CONFIG — edit these to taste
# ──────────────────────────────────────────────────────────────

# Offset from the switch center to the LED center, in mm.
# This is applied BEFORE rotation, so X is "to the right of the
# switch" and Y is "below the switch" when the switch is at 0°.
#
# For Choc V1 + SK6812-E reverse-mount, common starting points:
#   (0, 0)      — dead center of the switch
#   (0, -5.0)   — 5 mm above center (north, toward the stem)
# Adjust until the LED lands over the light pipe / window.
OFFSET_X_MM = 0.0
OFFSET_Y_MM = 0.0

# Set True to also copy the switch's rotation onto the LED.
COPY_ORIENTATION = True

# Switch → LED pairings: (LED ref, rotation)
#   rotation: 0 = no extra rotation
#             1 = 90° CW
#             2 = 180°
#             3 = 270° CW (90° CCW)
PAIRS = {
    "SW1": ("IC1", 0),
    "SW2": ("IC2", 0),
    "SW3": ("IC3", 0),
    "SW4": ("IC4", 0),
    "SW5": ("IC8", 2),
    "SW6": ("IC7", 2),
    "SW7": ("IC6", 2),
    "SW8": ("IC5", 2),
}

# ──────────────────────────────────────────────────────────────
# SCRIPT — no need to edit below
# ──────────────────────────────────────────────────────────────

board = pcbnew.GetBoard()

# Build a ref → footprint lookup
footprints = {fp.GetReference(): fp for fp in board.GetFootprints()}

for sw_ref, (led_ref, extra_rot) in PAIRS.items():
    sw = footprints.get(sw_ref)
    led = footprints.get(led_ref)

    if sw is None:
        print(f"WARNING: {sw_ref} not found on board, skipping")
        continue
    if led is None:
        print(f"WARNING: {led_ref} not found on board, skipping")
        continue

    # Switch position (in KiCad internal units — nanometers)
    sw_pos = sw.GetPosition()
    sw_angle_deg = sw.GetOrientationDegrees()
    sw_angle_rad = math.radians(sw_angle_deg)

    # Convert mm offset to nanometers
    dx_nm = pcbnew.FromMM(OFFSET_X_MM)
    dy_nm = pcbnew.FromMM(OFFSET_Y_MM)

    # Rotate the offset to match the switch orientation
    rot_dx = dx_nm * math.cos(sw_angle_rad) - dy_nm * math.sin(sw_angle_rad)
    rot_dy = dx_nm * math.sin(sw_angle_rad) + dy_nm * math.cos(sw_angle_rad)

    new_x = int(sw_pos.x + rot_dx)
    new_y = int(sw_pos.y + rot_dy)

    led.SetPosition(pcbnew.VECTOR2I(new_x, new_y))

    final_angle = sw_angle_deg + (extra_rot * 90.0)
    if COPY_ORIENTATION:
        led.SetOrientationDegrees(final_angle)

    print(f"{led_ref} → placed at {sw_ref} + offset "
          f"({OFFSET_X_MM}, {OFFSET_Y_MM}) mm  "
          f"[orientation {final_angle}° = switch {sw_angle_deg}° + {extra_rot * 90}°]")

# Refresh the canvas
pcbnew.Refresh()
print("\nDone! Ctrl+Z to undo if needed.")