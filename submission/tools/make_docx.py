import re
import struct
import sys
import zipfile
from pathlib import Path
from xml.sax.saxutils import escape

AUTHOR = "Mbazu Onyedikachi Victor"
MATRIC = "2025/A/SENG/0220"
COURSE = "COS202 Programming II - Lab Assessment Project"
TITLE = "Smart Clinic Appointment & Patient Management System (SCAPMS)"
EMU = 914400

NS = ('xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main" '
      'xmlns:r="http://schemas.openxmlformats.org/officeDocument/2006/relationships" '
      'xmlns:wp="http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing" '
      'xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main" '
      'xmlns:pic="http://schemas.openxmlformats.org/drawingml/2006/picture"')

REPORT = [
    ("1. Project description", ["SCAPMS is a desktop application written in C++17 with the Qt Widgets toolkit for a university clinic. Staff register patients, schedule and cancel appointments, manage a waiting queue, search and sort records, and read simple reports. Data is stored in a SQLite database and can also be imported from or exported to CSV files. The program is split into model, controller, GUI and utility layers so that the interface, the business rules and the data handling are never combined in one class."]),
    ("2. System features", ["Register, edit and delete patients (General, Emergency, FollowUp); schedule, cancel and complete appointments; waiting queue with a \"call next patient\" action; search by ID, name or category; sort by name, ID, age or appointment date with a selectable algorithm; undo of the last administrative operation (delete, edit, cancel); four reports (most frequently attended patients, patients with pending appointments, category distribution, daily appointment summary); a dashboard with live statistics, a cache of recently used patients and appointment reminders; save/load to SQLite and import/export to CSV through a file dialog."]),
    ("3. Class hierarchy", ["**Appointable** is an interface-like abstract class (scheduleAppointment, cancelAppointment). **PatientRecord** is the abstract base class holding ID, name, age, contact and a type-specific detail; it has the pure virtual functions category(), describe(), priorityLabel() and detailLabel(). **GeneralPatient**, **EmergencyPatient** and **FollowUpPatient** override them; FollowUpPatient also refuses appointments dated before the previous visit. Each PatientRecord owns one or more **Appointment** objects (composition) and **ClinicDatabase** holds the collection of patient records.",
                            "Runtime polymorphism is shown by ClinicManager::describeForConsultation(const PatientRecord&) and by the tables, which call describe() and priorityLabel() through the base type. Everything lives in the namespace scapms (with scapms::algo and scapms::constants), organised in model/, controller/ (ClinicManager, SearchEngine, AppointmentController), gui/ (MainWindow plus Dashboard, Patient, Appointment, Queue, Search, Reports and Admin panels) and utils/ (IDGenerator, FileHandler, DatabaseManager, Algorithms)."]),
    ("4. Data structures used", [("table", [
        ("std::vector", "Registered patients (ClinicDatabase) and each patient's appointments."),
        ("std::queue", "Waiting queue: the first patient added is called first; each entry stores the time it joined, which drives the waiting alerts."),
        ("std::stack", "Undo of the last administrative operation: deleting a patient, editing a patient, or cancelling an appointment."),
        ("Fixed array", "PatientRecord* cache[10], a ring buffer of \"frequently accessed patient records\", shown on the dashboard.")])]),
    ("5. Searching algorithms used", ["All searches are written by hand (no std::find). **Linear search** finds a patient by ID when the list is unsorted. **Binary search** is used when the list is currently sorted by ID (or, for an exact name, sorted by name). **Recursive search** finds patients whose name contains a keyword by examining one element and recursing on the rest. Category search uses a linear scan. The program records how the list was last sorted and chooses the technique automatically; the algorithm used is shown under the search box."]),
    ("6. Sorting algorithms used", ["**Selection sort** and **merge sort** are implemented manually and can be chosen from a combo box, together with the field to sort by: name, patient ID, age or next appointment date."]),
    ("7. Recursive algorithm used", ["recursiveSearchName() (name search) and mergeSort() (divide the list, sort both halves recursively, merge them)."]),
    ("8. GUI design", ["A welcome page (QStackedWidget) leads to a tabbed workspace: Dashboard, Patients, Appointments, Queue, Search & Sort, Reports and Admin. Layouts are used throughout instead of fixed positions: **QVBoxLayout** and **QHBoxLayout** arrange headings, toolbars and tables so they resize with the window; **QFormLayout** gives aligned label/field rows in the registration and appointment forms; **QGridLayout** places the dashboard statistic cards and admin buttons in rows and columns; **QStackedWidget** switches between the welcome page and the workspace.",
                       "Advanced techniques: colour-coded tables (emergency rows red and bold, follow-ups blue; appointment statuses amber, green and grey), dynamic form fields that change with the chosen patient category, a file dialog for CSV import and export, timers for appointment reminders, waiting-time alerts and dashboard refresh, live input validation with inline messages, keyboard shortcuts (Ctrl+S, Ctrl+F, Ctrl+Q, Ctrl+Z and others) and tooltips on the main controls. Events are handled with Qt signals and slots: each panel reports outcomes to MainWindow through changed, failed and notified signals."]),
    ("9. Exception handling approach", ["A custom exception, **ClinicException**, is thrown by the model and controller for invalid patient information, duplicate patient ID, age outside 0-120, invalid appointment data, file loading failures and invalid search input. Every GUI action runs inside Panel::run() or Panel::guard(), which catch the exception and show a message box instead of letting the program terminate. Validation is written once in the model (PatientRecord::validate, Appointment::isValidDate, PatientFactory::validate) and reused by the form for live feedback. Loading and importing build the new data on the side and swap it in only if everything is valid, so a bad file leaves the current records unchanged."]),
    ("10. Algorithms chosen and reasons", ["**Selection sort** is simple and makes at most n-1 swaps, which suits the small lists a clinic usually works with. **Merge sort** is O(n log n) in every case and stable (equal values keep their registration order), so it is the better choice as the list grows; the Search & Sort tab explains this and recommends one of them from the current patient count. **Linear search** is the only correct choice on unsorted data. **Binary search** needs a sorted list but then takes O(log n); the program uses it only after the matching sort. **Recursive search** expresses the partial name match naturally and works on unsorted data."]),
    ("11. Challenges encountered", ["(a) The first version put almost all interface code into a single MainWindow class, and the controller passed data to it as JSON text. (b) Binary search gives wrong answers if the list is no longer sorted, for example after a new patient is registered. (c) The same validation rules were needed in the form and in the model. (d) The existing database had no column for the type-specific detail. (e) Timer events such as reminders and queue alerts must not interrupt the user with modal dialogs. (f) Importing a faulty file could leave the system half loaded."]),
    ("12. How the challenges were resolved", ["(a) The interface was split into one panel per task with a shared Panel base class, and the controller now returns typed C++ objects. (b) ClinicManager remembers the field the list was last sorted by and clears it when records are added or restored, so binary search is used only when it is valid. (c) Validation lives in the model and the form calls it. (d) DatabaseManager adds the column automatically when it opens an older database. (e) Timers update labels, lists and the status bar. (f) Loading and importing validate into a temporary database and swap it in only on success."]),
]

SCREENSHOTS = [
    ("Main dashboard", ["01-dashboard.png"]),
    ("Patient registration: dynamic fields for the FollowUp category (left) and input validation message (right)", ["02-registration.png", "02b-registration-validation.png"]),
    ("Patient records (colour-coded by priority)", ["03-patient-directory.png"]),
    ("Appointment management", ["04-appointments.png"]),
    ("Search / sort interface (merge sort by name, then exact-name binary search)", ["05-search-and-sort.png"]),
    ("Waiting queue", ["06-waiting-queue.png"]),
    ("Reports", ["07-reports.png"]),
]


class Builder:
    def __init__(self):
        self.body = []
        self.media = []

    def run(self, text, bold=False, size=None, mono=False, color=None):
        props = ""
        if mono:
            props += '<w:rFonts w:ascii="Consolas" w:hAnsi="Consolas" w:cs="Courier New"/>'
        if bold:
            props += "<w:b/>"
        if color:
            props += f'<w:color w:val="{color}"/>'
        if size:
            props += f'<w:sz w:val="{size}"/><w:szCs w:val="{size}"/>'
        return f'<w:r><w:rPr>{props}</w:rPr><w:t xml:space="preserve">{escape(text)}</w:t></w:r>'

    def rich_runs(self, text):
        parts = re.split(r"\*\*(.+?)\*\*", text)
        return "".join(self.run(p, bold=(i % 2 == 1)) for i, p in enumerate(parts) if p)

    def paragraph(self, runs, style=None, align=None, page_break_before=False, keep_next=False):
        props = ""
        if style:
            props += f'<w:pStyle w:val="{style}"/>'
        if keep_next:
            props += "<w:keepNext/>"
        if page_break_before:
            props += "<w:pageBreakBefore/>"
        if align:
            props += f'<w:jc w:val="{align}"/>'
        return f"<w:p><w:pPr>{props}</w:pPr>{runs}</w:p>"

    def add(self, xml):
        self.body.append(xml)

    def heading(self, text, level, page_break=False):
        self.add(self.paragraph(self.run(text), style=f"Heading{level}", page_break_before=page_break, keep_next=True))

    def text(self, text, align=None):
        self.add(self.paragraph(self.rich_runs(text), align=align))

    def code(self, text):
        lines = text.expandtabs(4).splitlines() or [""]
        for line in lines:
            self.add(self.paragraph(self.run(line), style="Code"))

    def image_xml(self, path, width_in):
        data = Path(path).read_bytes()
        width_px, height_px = struct.unpack(">II", data[16:24])
        index = len(self.media) + 1
        self.media.append((f"image{index}.png", data))
        cx = int(width_in * EMU)
        cy = int(cx * height_px / width_px)
        return (f'<w:r><w:drawing><wp:inline distT="0" distB="0" distL="0" distR="0"><wp:extent cx="{cx}" cy="{cy}"/>'
                f'<wp:docPr id="{index}" name="Picture {index}"/><wp:cNvGraphicFramePr/>'
                f'<a:graphic><a:graphicData uri="http://schemas.openxmlformats.org/drawingml/2006/picture"><pic:pic>'
                f'<pic:nvPicPr><pic:cNvPr id="{index}" name="image{index}.png"/><pic:cNvPicPr/></pic:nvPicPr>'
                f'<pic:blipFill><a:blip r:embed="rIdImg{index}"/><a:stretch><a:fillRect/></a:stretch></pic:blipFill>'
                f'<pic:spPr><a:xfrm><a:off x="0" y="0"/><a:ext cx="{cx}" cy="{cy}"/></a:xfrm><a:prstGeom prst="rect"><a:avLst/></a:prstGeom></pic:spPr>'
                f'</pic:pic></a:graphicData></a:graphic></wp:inline></w:drawing></w:r>')

    def picture(self, path, width_in):
        self.add(self.paragraph(self.image_xml(path, width_in), align="center"))

    def picture_row(self, paths, width_in):
        cells = "".join(f'<w:tc><w:tcPr><w:tcW w:w="{int(9000 / len(paths))}" w:type="dxa"/></w:tcPr>{self.paragraph(self.image_xml(p, width_in), align="center")}</w:tc>' for p in paths)
        grid = "".join(f'<w:gridCol w:w="{int(9000 / len(paths))}"/>' for _ in paths)
        self.add(f'<w:tbl><w:tblPr><w:tblW w:w="0" w:type="auto"/></w:tblPr><w:tblGrid>{grid}</w:tblGrid><w:tr>{cells}</w:tr></w:tbl>')
        self.add(self.paragraph(""))

    def table(self, rows):
        border = '<w:tblBorders>' + "".join(f'<w:{s} w:val="single" w:sz="4" w:space="0" w:color="9FB3C8"/>' for s in ("top", "left", "bottom", "right", "insideH", "insideV")) + "</w:tblBorders>"
        xml = f'<w:tbl><w:tblPr><w:tblW w:w="9000" w:type="dxa"/>{border}<w:tblCellMar><w:left w:w="100" w:type="dxa"/><w:right w:w="100" w:type="dxa"/></w:tblCellMar></w:tblPr><w:tblGrid><w:gridCol w:w="2000"/><w:gridCol w:w="7000"/></w:tblGrid>'
        for left, right in rows:
            xml += ('<w:tr>'
                    f'<w:tc><w:tcPr><w:tcW w:w="2000" w:type="dxa"/><w:shd w:val="clear" w:color="auto" w:fill="E1EFFE"/></w:tcPr>{self.paragraph(self.run(left, bold=True))}</w:tc>'
                    f'<w:tc><w:tcPr><w:tcW w:w="7000" w:type="dxa"/></w:tcPr>{self.paragraph(self.run(right))}</w:tc></w:tr>')
        self.add(xml + "</w:tbl>")
        self.add(self.paragraph(""))


def listing(root, folder, patterns):
    files = []
    for pattern in patterns:
        files += sorted((root / folder).glob(pattern))
    return sorted(set(files), key=lambda p: p.name)


def build_content(root, output):
    b = Builder()
    images = output / "images"

    b.add(b.paragraph("", ))
    b.add(b.paragraph(b.run(COURSE, bold=True, size=36, color="102A43"), align="center"))
    b.add(b.paragraph(b.run(TITLE, size=28, color="102A43"), align="center"))
    b.add(b.paragraph(""))
    b.add(b.paragraph(b.run(f"Name: {AUTHOR}", bold=True, size=28), align="center"))
    b.add(b.paragraph(b.run(f"Matric number: {MATRIC}", bold=True, size=28), align="center"))
    b.add(b.paragraph(""))
    b.add(b.paragraph(b.run("Contents: 1. Source code   2. Screenshots   3. UML / class diagrams   4. Project report", size=22), align="center"))

    b.heading("Part 1: Source Code", 1, page_break=True)
    sections = [
        ("1. Header files", [p for folder in ("model", "controller", "utils") for p in listing(root, folder, ["*.h"])]),
        ("2. Source files", [root / "main.cpp"] + [p for folder in ("model", "controller", "utils") for p in listing(root, folder, ["*.cpp"])]),
        ("3. GUI files", listing(root, "gui", ["*.h", "*.cpp"])),
        ("4. Build file", [root / "CMakeLists.txt"]),
    ]
    for heading, files in sections:
        b.heading(heading, 2, page_break=True)
        for path in files:
            b.heading(str(path.relative_to(root)), 3)
            b.code(path.read_text())
    b.heading("5. Data files required to run the application", 2, page_break=True)
    b.text("The SQLite database scapms.db is created automatically in the working directory on first launch. A sample data set that can be loaded with File > Import is listed below.")
    b.heading("sample-data.csv", 3)
    b.code((output / "sample-data.csv").read_text())

    b.heading("Part 2: Screenshots", 1, page_break=True)
    for caption, names in SCREENSHOTS:
        b.heading(caption, 3)
        if len(names) == 1:
            b.picture(images / names[0], 6.3)
        else:
            b.picture_row([images / n for n in names], 3.0)

    b.heading("Part 3: UML / Class Diagrams", 1, page_break=True)
    b.heading("Class hierarchy: inheritance, abstract classes, polymorphism, composition", 3)
    b.picture(images / "uml-class-hierarchy.png", 6.5)
    b.heading("Application architecture", 3)
    b.picture(images / "uml-architecture.png", 6.5)

    b.heading("Part 4: Project Report", 1, page_break=True)
    b.add(b.paragraph(b.run(f"{TITLE} - Project Report", bold=True), align="center"))
    b.add(b.paragraph(b.run(f"{AUTHOR}  |  {MATRIC}"), align="center"))
    for heading, blocks in REPORT:
        b.heading(heading, 3)
        for block in blocks:
            if isinstance(block, tuple):
                b.table(block[1])
            else:
                b.text(block)
    return b


STYLES = f'''<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<w:styles {NS}>
<w:docDefaults><w:rPrDefault><w:rPr><w:rFonts w:ascii="Calibri" w:hAnsi="Calibri" w:cs="Calibri"/><w:sz w:val="21"/><w:szCs w:val="21"/></w:rPr></w:rPrDefault>
<w:pPrDefault><w:pPr><w:spacing w:after="80" w:line="259" w:lineRule="auto"/></w:pPr></w:pPrDefault></w:docDefaults>
<w:style w:type="paragraph" w:default="1" w:styleId="Normal"><w:name w:val="Normal"/></w:style>
<w:style w:type="paragraph" w:styleId="Heading1"><w:name w:val="heading 1"/><w:basedOn w:val="Normal"/><w:next w:val="Normal"/><w:pPr><w:keepNext/><w:spacing w:before="240" w:after="160"/><w:outlineLvl w:val="0"/></w:pPr><w:rPr><w:b/><w:color w:val="102A43"/><w:sz w:val="36"/></w:rPr></w:style>
<w:style w:type="paragraph" w:styleId="Heading2"><w:name w:val="heading 2"/><w:basedOn w:val="Normal"/><w:next w:val="Normal"/><w:pPr><w:keepNext/><w:spacing w:before="200" w:after="120"/><w:outlineLvl w:val="1"/></w:pPr><w:rPr><w:b/><w:color w:val="102A43"/><w:sz w:val="28"/></w:rPr></w:style>
<w:style w:type="paragraph" w:styleId="Heading3"><w:name w:val="heading 3"/><w:basedOn w:val="Normal"/><w:next w:val="Normal"/><w:pPr><w:keepNext/><w:spacing w:before="160" w:after="60"/><w:outlineLvl w:val="2"/></w:pPr><w:rPr><w:b/><w:color w:val="102A43"/><w:sz w:val="23"/></w:rPr></w:style>
<w:style w:type="paragraph" w:customStyle="1" w:styleId="Code"><w:name w:val="Code"/><w:basedOn w:val="Normal"/><w:pPr><w:spacing w:after="0" w:line="240" w:lineRule="auto"/></w:pPr><w:rPr><w:rFonts w:ascii="Consolas" w:hAnsi="Consolas" w:cs="Courier New"/><w:sz w:val="14"/><w:szCs w:val="14"/></w:rPr></w:style>
<w:style w:type="paragraph" w:styleId="Header"><w:name w:val="header"/><w:basedOn w:val="Normal"/></w:style>
<w:style w:type="paragraph" w:styleId="Footer"><w:name w:val="footer"/><w:basedOn w:val="Normal"/></w:style>
</w:styles>'''

HEADER = f'''<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<w:hdr {NS}><w:p><w:pPr><w:pStyle w:val="Header"/><w:jc w:val="right"/></w:pPr><w:r><w:rPr><w:sz w:val="18"/><w:color w:val="486581"/></w:rPr><w:t xml:space="preserve">{escape(AUTHOR)}  |  {escape(MATRIC)}  |  COS202 - SCAPMS</w:t></w:r></w:p></w:hdr>'''

FOOTER = f'''<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<w:ftr {NS}><w:p><w:pPr><w:pStyle w:val="Footer"/><w:jc w:val="center"/></w:pPr><w:r><w:rPr><w:sz w:val="18"/></w:rPr><w:t xml:space="preserve">Page </w:t></w:r><w:r><w:rPr><w:sz w:val="18"/></w:rPr><w:fldChar w:fldCharType="begin"/></w:r><w:r><w:rPr><w:sz w:val="18"/></w:rPr><w:instrText xml:space="preserve"> PAGE </w:instrText></w:r><w:r><w:rPr><w:sz w:val="18"/></w:rPr><w:fldChar w:fldCharType="separate"/></w:r><w:r><w:rPr><w:sz w:val="18"/></w:rPr><w:t>1</w:t></w:r><w:r><w:rPr><w:sz w:val="18"/></w:rPr><w:fldChar w:fldCharType="end"/></w:r></w:p></w:ftr>'''


def write_docx(builder, target):
    document = (f'<?xml version="1.0" encoding="UTF-8" standalone="yes"?><w:document {NS}><w:body>' + "".join(builder.body) +
                '<w:sectPr><w:headerReference w:type="default" r:id="rIdHeader"/><w:footerReference w:type="default" r:id="rIdFooter"/>'
                '<w:pgSz w:w="11906" w:h="16838"/><w:pgMar w:top="1134" w:right="1134" w:bottom="1134" w:left="1134" w:header="567" w:footer="567" w:gutter="0"/></w:sectPr></w:body></w:document>')
    relationships = ('<?xml version="1.0" encoding="UTF-8" standalone="yes"?><Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">'
                     '<Relationship Id="rIdStyles" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/styles" Target="styles.xml"/>'
                     '<Relationship Id="rIdHeader" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/header" Target="header1.xml"/>'
                     '<Relationship Id="rIdFooter" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/footer" Target="footer1.xml"/>' +
                     "".join(f'<Relationship Id="rIdImg{i}" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/image" Target="media/{name}"/>' for i, (name, _) in enumerate(builder.media, 1)) +
                     '</Relationships>')
    content_types = ('<?xml version="1.0" encoding="UTF-8" standalone="yes"?><Types xmlns="http://schemas.openxmlformats.org/package/2006/content-types">'
                     '<Default Extension="rels" ContentType="application/vnd.openxmlformats-package.relationships+xml"/>'
                     '<Default Extension="xml" ContentType="application/xml"/><Default Extension="png" ContentType="image/png"/>'
                     '<Override PartName="/word/document.xml" ContentType="application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml"/>'
                     '<Override PartName="/word/styles.xml" ContentType="application/vnd.openxmlformats-officedocument.wordprocessingml.styles+xml"/>'
                     '<Override PartName="/word/header1.xml" ContentType="application/vnd.openxmlformats-officedocument.wordprocessingml.header+xml"/>'
                     '<Override PartName="/word/footer1.xml" ContentType="application/vnd.openxmlformats-officedocument.wordprocessingml.footer+xml"/>'
                     '<Override PartName="/docProps/core.xml" ContentType="application/vnd.openxmlformats-package.core-properties+xml"/></Types>')
    root_rels = ('<?xml version="1.0" encoding="UTF-8" standalone="yes"?><Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">'
                 '<Relationship Id="rId1" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument" Target="word/document.xml"/>'
                 '<Relationship Id="rId2" Type="http://schemas.openxmlformats.org/package/2006/relationships/metadata/core-properties" Target="docProps/core.xml"/></Relationships>')
    core = ('<?xml version="1.0" encoding="UTF-8" standalone="yes"?><cp:coreProperties xmlns:cp="http://schemas.openxmlformats.org/package/2006/metadata/core-properties" '
            'xmlns:dc="http://purl.org/dc/elements/1.1/"><dc:title>SCAPMS - COS202 Lab Assessment</dc:title>'
            f'<dc:creator>{escape(AUTHOR)}</dc:creator></cp:coreProperties>')
    with zipfile.ZipFile(target, "w", zipfile.ZIP_DEFLATED) as z:
        z.writestr("[Content_Types].xml", content_types)
        z.writestr("_rels/.rels", root_rels)
        z.writestr("docProps/core.xml", core)
        z.writestr("word/document.xml", document)
        z.writestr("word/styles.xml", STYLES)
        z.writestr("word/header1.xml", HEADER)
        z.writestr("word/footer1.xml", FOOTER)
        z.writestr("word/_rels/document.xml.rels", relationships)
        for name, data in builder.media:
            z.writestr(f"word/media/{name}", data)


def main():
    root = Path(sys.argv[1]).resolve()
    output = root / "submission"
    write_docx(build_content(root, output), output / "SCAPMS_Submission.docx")


main()
