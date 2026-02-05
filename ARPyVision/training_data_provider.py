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
    original = cv2.imread(screenshot_file)
    if original is None:
        print(f"Error: Could not load image {screenshot_file}")
        return None

    # Create a copy for drawing lines on screenshot
    with_lines = original.copy()

    # Create black background for lines only
    lines_only = np.zeros_like(original)

    # Read lines from CSV and draw them
    if os.path.exists(frame_lines_file):
        lines = np.loadtxt(frame_lines_file, delimiter=',', ndmin=2)
        if lines.size > 0:
            for line in lines:
                x1, y1, x2, y2 = int(line[0]), int(line[1]), int(line[2]), int(line[3])
                cv2.line(with_lines, (x1, y1), (x2, y2), (0, 255, 0), 2)
                cv2.line(lines_only, (x1, y1), (x2, y2), (0, 255, 0), 2)

    combined = np.hstack((original, lines_only, with_lines))

    return combined


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


def transform_series_data(remove_diagonal_rectangle_lines):
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
        [TRANSFORMER_EXECUTABLE, raw_data_path, OUTPUT_LINES_DIR, str(remove_diagonal_rectangle_lines).lower()],
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


def clear_input_folder():
    """Clear all contents of the input folder."""
    if os.path.exists(INPUT_DIR):
        shutil.rmtree(INPUT_DIR)
        os.makedirs(INPUT_DIR, exist_ok=True)
        os.makedirs(OUTPUT_LINES_DIR, exist_ok=True)
        print("Input folder cleared.")


def post_process_training_data():
    training_data_dir = os.path.join(os.path.dirname(INPUT_DIR), "training_data")

    # Ask user if they want to use this as training data
    while True:
        choice = input("Do you want to use this series as training data? (y/n): ").strip().lower()
        if choice == "y":
            break
        if choice == "n":
            clear_input_folder()
            return

    # Ask for folder name and validate it doesn't exist
    while True:
        folder_name = input("Enter a name for the new training data folder: ").strip()
        if not folder_name:
            print("Folder name cannot be empty.")
            continue

        new_folder_path = os.path.join(training_data_dir, folder_name)
        if os.path.exists(new_folder_path):
            print(f"Folder '{folder_name}' already exists. Please choose a different name.")
            continue
        break

    # Create the folder structure
    images_folder = os.path.join(new_folder_path, "images")
    validation_lines_folder = os.path.join(new_folder_path, "validation_lines_2d")

    os.makedirs(images_folder, exist_ok=True)
    os.makedirs(validation_lines_folder, exist_ok=True)
    print(f"Created training data folder: {new_folder_path}")

    # Copy screenshots to images folder
    screenshot_files = glob.glob(os.path.join(INPUT_DIR, "screenshot_*.png"))
    for screenshot in screenshot_files:
        dest_path = os.path.join(images_folder, os.path.basename(screenshot))
        shutil.copy2(screenshot, dest_path)
    print(f"Copied {len(screenshot_files)} screenshots to images folder.")

    # Copy lines folder content to validation_lines_2d folder
    lines_folder = os.path.join(INPUT_DIR, "lines")
    if os.path.exists(lines_folder):
        lines_files = os.listdir(lines_folder)
        for f in lines_files:
            src_path = os.path.join(lines_folder, f)
            dest_path = os.path.join(validation_lines_folder, f)
            if os.path.isfile(src_path):
                shutil.copy2(src_path, dest_path)
        print(f"Copied {len(lines_files)} files to validation_lines_2d folder.")
    else:
        print("Warning: No lines folder found in input.")

    # Clear the input folder
    clear_input_folder()
    print(f"Training data '{folder_name}' created successfully.")

if __name__ == '__main__':
    transform_series_data(True)
    render_input()
    post_process_training_data()

