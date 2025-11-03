# Mechanical Fabrication Notes

Dimensions provided below are in millimetres unless otherwise stated. Tolerances ±0.5 mm unless noted.

## Mirror Carrier Plate

- Material: 3 mm aluminum 5052-H32.
- Overall size: 340 × 340 mm.
- Central cut-out: 150 × 150 mm to reduce weight.
- Mirror standoffs: three M5 PEM studs arranged on a 240 mm equilateral triangle.
- Edge clamps: four M4 tapped holes 15 mm from each corner for silicone bumpers.
- Hinge line: 50 mm offset from bottom edge with Ø8 mm bores for elevation trunnion.

## Elevation Bracket

- Side plates: 6 mm aluminum, 180 mm tall, slotted to accept mirror trunnion bearings (Ø8.02 mm reamed).
- Worm gearbox mounting pattern: 60 × 60 mm square, M6 clearance.
- Limit switch boss: 20 × 20 × 4 mm tab with M3 tapped hole 12 mm from pivot centre.
- Cable relief notch: 15 × 20 mm at rear to route motor wiring.

## Azimuth Base

- Base plate: 6 mm steel, 260 × 260 mm, centre hole Ø120 mm for lazy-Susan bearing.
- Bearing mounting: 8× M6 clearance holes on 220 mm bolt circle.
- Stepper bracket: 5 mm steel angle, slots for GT2 belt tension (20 mm adjustment).
- Hard stop posts: two M8 studs at ±140° relative to North reference.

## Belt Guard

- 2 mm polycarbonate wrap, 200 mm circumference × 40 mm width.
- Mount with M3 hardware to azimuth base plate tabs.

## Limit Switch Flags

- Elevation: 2 mm stainless tab, 20 mm reach, mount to mirror carrier via M3 screw.
- Azimuth: adjustable aluminum angle, 30 × 20 × 2 mm, slot for ±10° tuning.

## Drawing Package Export

1. Import the coordinate tables into CAD (DXF/STEP) using your template of choice.
2. Add bend radii (2 mm) for the mirror carrier flanges if forming sheet metal lips.
3. Generate PDF and DXF deliverables and store them inside `docs/docs_assets/`.
4. Update hole callouts with hardware finish (zinc-plated steel or stainless).

## Fastener Summary

| Location | Hardware | Qty |
| --- | --- | --- |
| Mirror standoffs | M5 × 20 socket cap + nyloc | 3 |
| Mirror clamp corners | M4 × 16 pan head + nyloc | 4 |
| Elevation gearbox | M6 × 25 cap screws | 4 |
| Azimuth bearing | M6 × 16 countersunk | 8 |
| Limit switch mounts | M3 × 12 pan head | 6 |

## Finishing Notes

- Deburr all edges to 0.5 mm chamfer.
- Apply etch primer and powder coat (RAL 7035) for corrosion resistance.
- Use threadlocker (medium) on all vibration-prone fasteners.
