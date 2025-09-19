import os
import shutil
import sys
from datetime import datetime

if len(sys.argv) > 1:
    src = sys.argv[1]
else:
    src = "."

month_to_quarter = {
    1: "wiosna", 2: "wiosna", 3: "wiosna",
    4: "lato", 5: "lato", 6: "lato",
    7: "jesien", 8: "jesien", 9: "jesien",
    10: "zima", 11: "zima", 12: "zima"
}
for root, dirs, files in os.walk(src):
    for filename in files:
        filepath = os.path.join(root, filename)
        mtime = datetime.fromtimestamp(os.path.getmtime(filepath))
        print(mtime.strftime("%m/%d/%Y %H:%M:%S"))
        year = mtime.year
        quarter = month_to_quarter[mtime.month]
        destination = os.path.join(src, str(str(year) + "-" + quarter))
        os.makedirs(destination, exist_ok=True)
        shutil.move(filepath, destination)
        print(filepath)
