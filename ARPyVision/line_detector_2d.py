import os
import glob
import numpy as np
import cv2

TRAINING_DATA_DIR = "/Users/cmoeller/dev/projects/ARVision/ARPyVision/training_data"


def select_training_data_folder():
    """Let user select a folder from training_data directory."""
    if not os.path.exists(TRAINING_DATA_DIR):
        print(f"Training data directory not found: {TRAINING_DATA_DIR}")
        return None

    # Get all subfolders
    subfolders = [f for f in os.listdir(TRAINING_DATA_DIR)
                  if os.path.isdir(os.path.join(TRAINING_DATA_DIR, f)) and not f.startswith('.')]
    subfolders.sort()

    if not subfolders:
        print(f"No training data folders found in {TRAINING_DATA_DIR}")
        return None

    # Display options
    print("Available training data folders:")
    for i, folder in enumerate(subfolders, 1):
        print(f"  {i}. {folder}")

    # Ask user to select
    while True:
        try:
            choice = input(f"Select folder (1-{len(subfolders)}): ")
            index = int(choice) - 1
            if 0 <= index < len(subfolders):
                return os.path.join(TRAINING_DATA_DIR, subfolders[index])
            else:
                print(f"Please enter a number between 1 and {len(subfolders)}")
        except ValueError:
            print("Please enter a valid number")


def get_available_frames(training_folder):
    """Get list of available frame numbers from images folder."""
    images_folder = os.path.join(training_folder, "images")
    screenshot_files = glob.glob(os.path.join(images_folder, "screenshot_*.png"))
    frames = []
    for f in screenshot_files:
        basename = os.path.basename(f)
        # Extract frame number from screenshot_X.png
        num = int(basename.replace("screenshot_", "").replace(".png", ""))
        frames.append(num)
    frames.sort()
    return frames


def render_frame(training_folder, frame_number):
    """Render a frame with original image on left and validation lines on right."""
    images_folder = os.path.join(training_folder, "images")
    lines_folder = os.path.join(training_folder, "validation_lines_2d")

    screenshot_file = os.path.join(images_folder, f"screenshot_{frame_number}.png")
    lines_file = os.path.join(lines_folder, f"frame_{frame_number}_lines.csv")

    # Load the screenshot image
    original = cv2.imread(screenshot_file)
    if original is None:
        print(f"Error: Could not load image {screenshot_file}")
        return None

    # Create a copy for drawing validation lines
    validation_image = original.copy()

    # Read lines from CSV and draw them
    if os.path.exists(lines_file):
        lines = np.loadtxt(lines_file, delimiter=',', ndmin=2)
        if lines.size > 0:
            for line in lines:
                x1, y1, x2, y2 = int(line[0]), int(line[1]), int(line[2]), int(line[3])
                cv2.line(validation_image, (x1, y1), (x2, y2), (0, 255, 0), 2)

    # Combine original (left) and validation image (right)
    combined = np.hstack((original, validation_image))

    return combined


def run_viewer():
    """Main viewer loop for browsing training data."""
    training_folder = select_training_data_folder()
    if training_folder is None:
        return

    frames = get_available_frames(training_folder)
    if not frames:
        print("No frames available in selected training data folder")
        return

    current_index = 0
    window_name = "Line Detector 2D"

    print(f"Found {len(frames)} frames. Use LEFT/RIGHT arrow keys to navigate, ESC or 'q' to quit.")

    while True:
        frame_number = frames[current_index]
        image = render_frame(training_folder, frame_number)

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


if __name__ == '__main__':
    run_viewer()
