from pathlib import Path
from textwrap import dedent

OUTPUT_DIR = Path(__file__).resolve().parent


def write_file(path: Path, content: str, binary: bool = False):
    mode = "wb" if binary else "w"
    with path.open(mode) as handle:
        handle.write(content if not binary else content.encode("latin-1"))


def create_wiring_schematic_svg():
    svg = dedent(
        """
        <svg xmlns="http://www.w3.org/2000/svg" width="1100" height="850" viewBox="0 0 1100 850">
          <style>
            .box { fill: #f0f4ff; stroke: #1f3b4d; stroke-width: 3; rx: 20; ry: 20; }
            .title { font: bold 28px sans-serif; fill: #1f3b4d; }
            .label { font: 24px sans-serif; fill: #0a1633; }
            .note { font: 20px sans-serif; fill: #0a1633; }
          </style>
          <rect x="0" y="0" width="1100" height="850" fill="#ffffff"/>
          <text x="550" y="60" class="title" text-anchor="middle">Heliostat Wiring Schematic</text>

          <rect x="80" y="140" width="200" height="150" class="box" fill="#ffe7d6"/>
          <text x="180" y="200" class="label" text-anchor="middle">Solar Array</text>
          <text x="180" y="230" class="label" text-anchor="middle">+ Battery 24V</text>

          <rect x="420" y="140" width="300" height="150" class="box" fill="#e4f5f9"/>
          <text x="570" y="200" class="label" text-anchor="middle">Main Controller</text>
          <text x="570" y="230" class="label" text-anchor="middle">(MCU + Power Mgmt)</text>

          <rect x="820" y="140" width="200" height="150" class="box" fill="#e6f4ea"/>
          <text x="920" y="200" class="label" text-anchor="middle">Motor Drivers</text>
          <text x="920" y="230" class="label" text-anchor="middle">AZ &amp; EL</text>

          <rect x="110" y="360" width="220" height="120" class="box" fill="#fdebd0"/>
          <text x="220" y="410" class="label" text-anchor="middle">Safety Interlocks</text>
          <text x="220" y="440" class="label" text-anchor="middle">(E-Stop, Fusing)</text>

          <rect x="450" y="360" width="220" height="120" class="box" fill="#fff0f6"/>
          <text x="560" y="410" class="label" text-anchor="middle">Limit Switches</text>
          <text x="560" y="440" class="label" text-anchor="middle">AZ+/-, EL+/-</text>

          <rect x="820" y="360" width="200" height="120" class="box" fill="#e8f8f5"/>
          <text x="920" y="410" class="label" text-anchor="middle">Encoders</text>
          <text x="920" y="440" class="label" text-anchor="middle">(Feedback)</text>

          <rect x="80" y="560" width="240" height="120" class="box" fill="#fef9e7"/>
          <text x="200" y="610" class="label" text-anchor="middle">GPS Receiver</text>
          <text x="200" y="640" class="label" text-anchor="middle">UART</text>

          <rect x="420" y="560" width="240" height="120" class="box" fill="#f4ecf7"/>
          <text x="540" y="610" class="label" text-anchor="middle">LCD Panel</text>
          <text x="540" y="640" class="label" text-anchor="middle">I2C</text>

          <rect x="780" y="560" width="240" height="120" class="box" fill="#ebf5fb"/>
          <text x="900" y="610" class="label" text-anchor="middle">Joystick Panel</text>
          <text x="900" y="640" class="label" text-anchor="middle">Analog + PB</text>

          <line x1="280" y1="210" x2="420" y2="210" stroke="#c0392b" stroke-width="6"/>
          <line x1="720" y1="210" x2="820" y2="210" stroke="#c0392b" stroke-width="6"/>
          <line x1="180" y1="290" x2="180" y2="360" stroke="#c0392b" stroke-width="4"/>
          <line x1="220" y1="480" x2="560" y2="480" stroke="#2e86c1" stroke-width="4" stroke-dasharray="12,12"/>
          <line x1="560" y1="480" x2="920" y2="480" stroke="#196f3d" stroke-width="4"/>
          <line x1="540" y1="560" x2="540" y2="480" stroke="#2e86c1" stroke-width="4"/>
          <line x1="200" y1="560" x2="540" y2="480" stroke="#2e86c1" stroke-width="4"/>
          <line x1="900" y1="560" x2="560" y2="480" stroke="#2e86c1" stroke-width="4"/>

          <text x="550" y="330" class="note" text-anchor="middle">24V DC Bus across power modules</text>
          <text x="550" y="520" class="note" text-anchor="middle">Control Backbone (I2C / UART / Analog)</text>
          <text x="550" y="780" class="note" text-anchor="middle">Shield encoder &amp; limit switch runs; route interlocks ahead of drivers.</text>
        </svg>
        """
    ).strip()
    write_file(OUTPUT_DIR / "wiring_schematic.svg", svg)


def create_mechanical_svgs():
    parts = [
        ("mirror_carrier", "Mirror Carrier", "450 x 320 x 8 mm plate with 4x Ø8 slots (40 mm long) and rear stiffening ribs."),
        ("elevation_bracket", "Elevation Bracket", "200 x 160 x 10 mm with central Ø25 pivot boss and 45° gussets."),
        ("azimuth_base", "Azimuth Base", "260 x 260 x 12 mm base plate, Ø220 clearance, 4x Ø14 anchors."),
        ("belt_guard", "Belt Guard", "220 x 120 x 2 mm folded cover, perforated ventilation, 20 mm clearance."),
        ("limit_switch_mount", "Limit Switch Mount", "90 x 60 x 4 mm slotted plate for fine switch placement."),
    ]

    base_svg = """
    <svg xmlns=\"http://www.w3.org/2000/svg\" width=\"800\" height=\"500\" viewBox=\"0 0 800 500\">
      <style>
        .title {{ font: bold 28px sans-serif; }}
        .body {{ font: 22px sans-serif; }}
      </style>
      <rect x=\"0\" y=\"0\" width=\"800\" height=\"500\" fill=\"#ffffff\" stroke=\"#0a1633\" stroke-width=\"4\"/>
      <rect x=\"150\" y=\"120\" width=\"500\" height=\"220\" fill=\"#f8f8f8\" stroke=\"#0a1633\" stroke-width=\"4\" rx=\"20\" ry=\"20\"/>
      <circle cx=\"250\" cy=\"200\" r=\"20\" fill=\"#ffffff\" stroke=\"#0a1633\" stroke-width=\"3\"/>
      <circle cx=\"400\" cy=\"200\" r=\"20\" fill=\"#ffffff\" stroke=\"#0a1633\" stroke-width=\"3\"/>
      <circle cx=\"550\" cy=\"200\" r=\"20\" fill=\"#ffffff\" stroke=\"#0a1633\" stroke-width=\"3\"/>
      <text x=\"400\" y=\"80\" class=\"title\" text-anchor=\"middle\">{title}</text>
      <text x=\"400\" y=\"370\" class=\"body\" text-anchor=\"middle\">{notes}</text>
    </svg>
    """

    for slug, title, notes in parts:
        svg = base_svg.format(title=title, notes=notes)
        write_file(OUTPUT_DIR / f"mechanical_drawing_{slug}.svg", svg.strip())


def create_pdf(filename: str, lines: list[str]):
    header = "%PDF-1.4\n"
    objects = []

    # Font object
    font_obj = "5 0 obj<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica >>endobj\n"

    text_lines = []
    y = 760
    for line in lines:
        if line == "":
            y -= 20
            continue
        safe_line = line.replace("(", "[" ).replace(")", "]")
        text_lines.append(f"BT /F1 12 Tf 72 {y} Td ({safe_line}) Tj ET\n")
        y -= 18

    content_stream = "".join(text_lines)
    contents = f"4 0 obj<< /Length {len(content_stream)} >>stream\n{content_stream}endstream endobj\n"

    page_obj = "3 0 obj<< /Type /Page /Parent 2 0 R /Resources<< /Font<< /F1 5 0 R>>>> /Contents 4 0 R /MediaBox[0 0 612 792]>>endobj\n"
    pages_obj = "2 0 obj<< /Type /Pages /Kids [3 0 R] /Count 1 >>endobj\n"
    catalog_obj = "1 0 obj<< /Type /Catalog /Pages 2 0 R >>endobj\n"

    xref_offset = len(header)
    body = catalog_obj + pages_obj + page_obj + contents + font_obj
    xref_start = xref_offset + len(body)

    xref = "xref\n0 6\n0000000000 65535 f \n"
    offsets = []
    current = xref_offset
    for obj in [catalog_obj, pages_obj, page_obj, contents, font_obj]:
        offsets.append(current)
        current += len(obj)
    for off in offsets:
        xref += f"{off:010d} 00000 n \n"

    trailer = "trailer<< /Size 6 /Root 1 0 R >>\nstartxref\n" + str(xref_start) + "\n%%EOF"

    pdf_content = header + body + xref + trailer
    write_file(OUTPUT_DIR / filename, pdf_content, binary=True)


def create_wiring_pdf():
    lines = [
        "Heliostat Wiring Schematic Summary",
        "",
        "Power: Solar array feeds 24V DC bus via safety interlock relay block.",
        "Battery and solar combiner include inline fusing and emergency stop loop.",
        "Motor drivers (azimuth & elevation) share the 24V bus downstream of interlocks.",
        "",
        "Control: Main controller distributes I2C and UART backbone to peripherals.",
        "GPS receiver connects on dedicated UART; LCD status panel resides on I2C.",
        "Joystick axes feed MCU analog inputs; pushbuttons wired as digital inputs.",
        "Limit switches (AZ+/-, EL+/-) return to MCU GPIO with shielded cabling.",
        "Motor encoders route through drivers for closed-loop feedback and MCU telemetry.",
        "",
        "Safety: Interlock relay removes power from drivers when any E-stop is triggered.",
        "Door switches and overcurrent sensors wired in series within interlock chain.",
        "Ground fault detection bonded to chassis and controller shield reference.",
    ]
    create_pdf("wiring_schematic.pdf", lines)


def create_mechanical_pdf():
    lines = [
        "Mechanical Drawing Notes",
        "",
        "Mirror Carrier: 450 x 320 x 8 mm aluminum plate, 4x Ø8 slotted holes (40 mm long).",
        "Bond 20 mm deep ribs at rear for stiffness; maintain flatness &lt;0.5 mm.",
        "",
        "Elevation Bracket: 200 x 160 x 10 mm plate with central Ø25 pivot boss.",
        "Add 45° gussets between base and side plates; provide grease channel.",
        "",
        "Azimuth Base: 260 x 260 x 12 mm base ring with Ø220 clearance for slew bearing.",
        "Anchor pattern Ø14 mm on 200 mm square bolt circle; integrate cable window.",
        "",
        "Belt Guard: 220 x 120 x 2 mm folded cover, louvered ventilation each side.",
        "Hinge on lower flange, tool-less fasteners on top inspection door.",
        "",
        "Limit Switch Mount: 90 x 60 x 4 mm stainless plate with 6 x 25 mm slots.",
        "Provide datum shoulder to align roller levers; include drip edge shield.",
    ]
    create_pdf("mechanical_drawings.pdf", lines)


def main():
    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
    create_wiring_schematic_svg()
    create_mechanical_svgs()
    create_wiring_pdf()
    create_mechanical_pdf()


if __name__ == "__main__":
    main()
