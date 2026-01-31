import os
import subprocess
import shutil
import glob
import numpy as np
import cv2

RAW_SERIES_DATA = "../simulator/arv-studio/recordings/series"
TRANSFORMER_EXECUTABLE = "/Users/cmoeller/dev/projects/ARVision/ARVTrainingDataTransformer/build/bin/Debug/ARVTrainingDataTransformer"
OUTPUT_LINES_DIR = "/Users/cmoeller/dev/projects/ARVision/ARPyVision/input/lines"
INPUT_DIR = "/Users/cmoeller/dev/projects/ARVision/ARPyVision/input"

def get_available_frames():
    screenshot_files = glob.glob(os.path.join(INPUT_DIR, "screenshot_*.png"))
    frames = []
    for f in screenshot_files:
        basename = os.path.basename(f)
        # Extract frame number from screenshot_X.png
        num = int(basename.replace("screenshot_", "").replace(".png", ""))
        frames.append(num)
    frames.sort()
    return frames


def render_frame(frame_number):
    frame_lines_file = os.path.join("input", "lines", f"frame_{frame_number}_lines.csv")
    screenshot_file = os.path.join("input", f"screenshot_{frame_number}.png")

    # Load the screenshot image
    image = cv2.imread(screenshot_file)
    if image is None:
        print(f"Error: Could not load image {screenshot_file}")
        return None

    # Read lines from CSV and draw them
    if os.path.exists(frame_lines_file):
        lines = np.loadtxt(frame_lines_file, delimiter=',')
        if lines.ndim == 1:
            lines = lines.reshape(1, -1)

        for line in lines:
            x1, y1, x2, y2 = int(line[0]), int(line[1]), int(line[2]), int(line[3])
            cv2.line(image, (x1, y1), (x2, y2), (0, 255, 0), 2)

    return image


def render_input():
    frames = get_available_frames()
    if not frames:
        print("No frames available in input folder")
        return

    current_index = 0
    window_name = "Lines Overlay"

    print(f"Found {len(frames)} frames. Use LEFT/RIGHT arrow keys to navigate, ESC or 'q' to quit.")

    while True:
        frame_number = frames[current_index]
        image = render_frame(frame_number)

        if image is None:
            break

        # Add frame info overlay
        cv2.putText(image, f"Frame {frame_number} ({current_index + 1}/{len(frames)})",
                    (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)

        cv2.imshow(window_name, image)

        key = cv2.waitKey(0) & 0xFF

        if key == 27 or key == ord('q'):  # ESC or 'q'
            break
        elif key == 81 or key == 2 or key == ord('a'):  # Left arrow or 'a'
            current_index = max(0, current_index - 1)
        elif key == 83 or key == 3 or key == ord('d'):  # Right arrow or 'd'
            current_index = min(len(frames) - 1, current_index + 1)

    cv2.destroyAllWindows()

def select_series_folder():
    series_base_path = os.path.abspath(RAW_SERIES_DATA)

    # Get all subfolders
    subfolders = [f for f in os.listdir(series_base_path)
                  if os.path.isdir(os.path.join(series_base_path, f)) and not f.startswith('.')]
    subfolders.sort()

    if not subfolders:
        print(f"No series folders found in {series_base_path}")
        return None

    # Display options
    print("Available series folders:")
    for i, folder in enumerate(subfolders, 1):
        print(f"  {i}. {folder}")

    # Ask user to select
    while True:
        try:
            choice = input(f"Select folder (1-{len(subfolders)}): ")
            index = int(choice) - 1
            if 0 <= index < len(subfolders):
                return os.path.join(series_base_path, subfolders[index])
            else:
                print(f"Please enter a number between 1 and {len(subfolders)}")
        except ValueError:
            print("Please enter a valid number")


def transform_series_data():
    raw_data_path = select_series_folder()
    if raw_data_path is None:
        return

    print(f"Transform series data {raw_data_path}")

    # Clean input folder before transformation
    if os.path.exists(INPUT_DIR):
        print(f"Cleaning input folder: {INPUT_DIR}")
        shutil.rmtree(INPUT_DIR)

    # Ensure output directories exist
    os.makedirs(OUTPUT_LINES_DIR, exist_ok=True)
    os.makedirs(INPUT_DIR, exist_ok=True)

    # Run the C++ transformer application
    print(f"Running transformer: {TRANSFORMER_EXECUTABLE}")
    print(f"  Input: {raw_data_path}")
    print(f"  Output: {OUTPUT_LINES_DIR}")

    process = subprocess.Popen(
        [TRANSFORMER_EXECUTABLE, raw_data_path, OUTPUT_LINES_DIR],
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        bufsize=1
    )

    for line in process.stdout:
        print(line, end='')

    process.wait()

    if process.returncode != 0:
        print(f"Transformer failed with return code {process.returncode}")
        return

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
    render_input()

