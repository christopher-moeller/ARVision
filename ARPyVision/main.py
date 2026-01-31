import os
import subprocess
import shutil
import glob
import numpy as np
import cv2

RAW_SERIES_DATA = "../simulator/arv-studio/recordings/series/main_scene_20260130_115021"
TRANSFORMER_EXECUTABLE = "/Users/cmoeller/dev/projects/ARVision/ARVTrainingDataTransformer/build/bin/Debug/ARVTrainingDataTransformer"
OUTPUT_LINES_DIR = "/Users/cmoeller/dev/projects/ARVision/ARPyVision/input/lines"
INPUT_DIR = "/Users/cmoeller/dev/projects/ARVision/ARPyVision/input"

def render_input(frame_number):
    print(f"Render frame {frame_number}")

    frame_lines_file = os.path.join("input", "lines", f"frame_{frame_number}_lines.csv")
    screenshot_file = os.path.join("input", f"screenshot_{frame_number}.png")

    # Load the screenshot image
    image = cv2.imread(screenshot_file)
    if image is None:
        print(f"Error: Could not load image {screenshot_file}")
        return

    # Read lines from CSV and draw them
    lines = np.loadtxt(frame_lines_file, delimiter=',')
    if lines.ndim == 1:
        lines = lines.reshape(1, -1)

    for line in lines:
        x1, y1, x2, y2 = int(line[0]), int(line[1]), int(line[2]), int(line[3])
        cv2.line(image, (x1, y1), (x2, y2), (0, 255, 0), 2)

    # Display the result
    cv2.imshow("Lines Overlay", image)
    cv2.waitKey(0)
    cv2.destroyAllWindows()

def transform_series_data():
    print(f"Transform series data {RAW_SERIES_DATA}")

    # Resolve the RAW_SERIES_DATA path to absolute path
    raw_data_path = os.path.abspath(RAW_SERIES_DATA)

    # Ensure output directories exist
    os.makedirs(OUTPUT_LINES_DIR, exist_ok=True)
    os.makedirs(INPUT_DIR, exist_ok=True)

    # Run the C++ transformer application
    print(f"Running transformer: {TRANSFORMER_EXECUTABLE}")
    print(f"  Input: {raw_data_path}")
    print(f"  Output: {OUTPUT_LINES_DIR}")

    result = subprocess.run(
        [TRANSFORMER_EXECUTABLE, raw_data_path, OUTPUT_LINES_DIR],
        capture_output=True,
        text=True
    )

    if result.returncode != 0:
        print(f"Transformer failed with return code {result.returncode}")
        print(f"stderr: {result.stderr}")
        return

    print(f"Transformer output: {result.stdout}")

    # Copy screenshots from RAW_SERIES_DATA to input folder
    print(f"Copying screenshots from {raw_data_path} to {INPUT_DIR}")
    screenshot_files = glob.glob(os.path.join(raw_data_path, "screenshot_*.png"))

    for screenshot in screenshot_files:
        dest_path = os.path.join(INPUT_DIR, os.path.basename(screenshot))
        shutil.copy2(screenshot, dest_path)
        print(f"  Copied {os.path.basename(screenshot)}")

    print(f"Transformation complete. Copied {len(screenshot_files)} screenshots.")

if __name__ == '__main__':
    transform_series_data()
    render_input(1)

