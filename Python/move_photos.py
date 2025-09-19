import os
import shutil
import sys
from datetime import datetime

if len(sys.argv) > 1:
    src = os.path.abspath(sys.argv[1])
else:
    src = os.path.abspath(".")

month_to_quarter = {
    1: "1-2-3", 2: "1-2-3", 3: "1-2-3",
    4: "4-5-6", 5: "4-5-6", 6: "4-5-6",
    7: "7-8-9", 8: "7-8-9", 9: "7-8-9",
    10: "10-11-12", 11: "10-11-12", 12: "10-11-12"
}
others = os.path.join(src, "Inne")
os.makedirs(others, exist_ok=True)
for root, dirs, files in os.walk(src):
    for filename in files:
        filepath = os.path.join(root, filename)
        if filepath == os.path.abspath(__file__): #pomin skrypt
            continue

        mtime = datetime.fromtimestamp(os.path.getmtime(filepath))
        print(mtime.strftime("%m/%d/%Y %H:%M:%S"))
        year = mtime.year
        quarter = month_to_quarter[mtime.month]
        destination = None
        if filename.endswith((".jpg", ".png", ".jpeg", ".avi", ".mp4", ".mov", ".wmv")):
            destination = os.path.join(src, f"{year}-{quarter}")
        else:
            destination = os.path.join(src, "Inne" ,f"{year}-{quarter}")

        os.makedirs(destination, exist_ok=True)
        shutil.move(filepath, os.path.join(destination, filename))
        print(f"Moved: {filepath} -> {destination}")

