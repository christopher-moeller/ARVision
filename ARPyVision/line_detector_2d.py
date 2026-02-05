import os
import glob
import json
import argparse
import numpy as np
import cv2
from abc import ABC, abstractmethod
from typing import Optional, List, Tuple

TRAINING_DATA_DIR = "/Users/cmoeller/dev/projects/ARVision/ARPyVision/training_data"


class LineDetectionExperiment(ABC):
    """Base class for line detection experiments."""

    @property
    @abstractmethod
    def name(self) -> str:
        """Name of the experiment (displayed as label)."""
        pass

    @abstractmethod
    def detect_lines(self, image: np.ndarray) -> Tuple[np.ndarray, np.ndarray]:
        """
        Detect lines in the input image.

        Args:
            image: Original input image (BGR format from OpenCV)

        Returns:
            Tuple of (visualization_image, predicted_lines)
            - visualization_image: BGR image showing the detection result
            - predicted_lines: numpy array of lines [[x1, y1, x2, y2], ...]
        """
        pass


class Experiment1(LineDetectionExperiment):
    """Detect lines using Canny edge detection followed by Hough transform."""

    def __init__(self, canny_low=50, canny_high=150, hough_threshold=50,
                 min_line_length=50, max_line_gap=10):
        self.canny_low = canny_low
        self.canny_high = canny_high
        self.hough_threshold = hough_threshold
        self.min_line_length = min_line_length
        self.max_line_gap = max_line_gap

    @property
    def name(self) -> str:
        return "Experiment 1"

    def detect_lines(self, image: np.ndarray) -> Tuple[np.ndarray, np.ndarray]:
        # Convert to grayscale
        gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

        # Apply Canny edge detection
        edges = cv2.Canny(gray, self.canny_low, self.canny_high)

        # Detect lines using Hough transform
        lines = cv2.HoughLinesP(
            edges,
            rho=1,
            theta=np.pi / 180,
            threshold=self.hough_threshold,
            minLineLength=self.min_line_length,
            maxLineGap=self.max_line_gap
        )

        # Create visualization (black background with green lines)
        output = np.zeros_like(image)

        predicted_lines = np.array([])
        if lines is not None:
            predicted_lines = np.array([line[0] for line in lines])
            for line in lines:
                x1, y1, x2, y2 = line[0]
                cv2.line(output, (x1, y1), (x2, y2), (0, 255, 0), 2)

        return output, predicted_lines


class Experiment2(LineDetectionExperiment):
    """Detect lines using Sobel edge detection followed by Hough transform."""

    def __init__(self, sobel_ksize=3, threshold_value=50, hough_threshold=50,
                 min_line_length=50, max_line_gap=10):
        self.sobel_ksize = sobel_ksize
        self.threshold_value = threshold_value
        self.hough_threshold = hough_threshold
        self.min_line_length = min_line_length
        self.max_line_gap = max_line_gap

    @property
    def name(self) -> str:
        return "Experiment 2"

    def detect_lines(self, image: np.ndarray) -> Tuple[np.ndarray, np.ndarray]:
        # Convert to grayscale
        gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

        # Apply Sobel edge detection
        sobel_x = cv2.Sobel(gray, cv2.CV_64F, 1, 0, ksize=self.sobel_ksize)
        sobel_y = cv2.Sobel(gray, cv2.CV_64F, 0, 1, ksize=self.sobel_ksize)
        sobel_mag = np.sqrt(sobel_x ** 2 + sobel_y ** 2)
        sobel_mag = np.uint8(sobel_mag / sobel_mag.max() * 255)

        # Threshold to get binary edge image
        _, edges = cv2.threshold(sobel_mag, self.threshold_value, 255, cv2.THRESH_BINARY)

        # Detect lines using Hough transform
        lines = cv2.HoughLinesP(
            edges,
            rho=1,
            theta=np.pi / 180,
            threshold=self.hough_threshold,
            minLineLength=self.min_line_length,
            maxLineGap=self.max_line_gap
        )

        # Create visualization (black background with green lines)
        output = np.zeros_like(image)

        predicted_lines = np.array([])
        if lines is not None:
            predicted_lines = np.array([line[0] for line in lines])
            for line in lines:
                x1, y1, x2, y2 = line[0]
                cv2.line(output, (x1, y1), (x2, y2), (0, 255, 0), 2)

        return output, predicted_lines


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


IMAGES_PER_ROW = 2
BORDER_THICKNESS = 10


def add_label(image, label):
    """Add a label to the top-left corner of an image."""
    labeled = image.copy()
    cv2.putText(labeled, label, (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 0), 2)
    return labeled


def add_metrics_to_image(image, num_predicted, num_validation):
    """Add metrics text at the bottom of an image."""
    result = image.copy()
    h = result.shape[0]

    diff = num_predicted - num_validation
    diff_str = f"+{diff}" if diff > 0 else str(diff)
    diff_color = (0, 255, 0) if diff == 0 else (0, 165, 255)

    font = cv2.FONT_HERSHEY_SIMPLEX
    font_scale = 0.6
    thickness = 2

    # Draw background rectangle for better readability
    cv2.rectangle(result, (0, h - 50), (result.shape[1], h), (0, 0, 0), -1)

    # Draw metrics text
    cv2.putText(result, f"Predicted: {num_predicted}", (10, h - 30),
                font, font_scale, (0, 255, 0), thickness)
    cv2.putText(result, f"Diff: {diff_str}", (10, h - 10),
                font, font_scale, diff_color, thickness)

    return result


def add_border(image, color, thickness=BORDER_THICKNESS):
    """Add a colored border around an image."""
    bordered = image.copy()
    h, w = bordered.shape[:2]
    cv2.rectangle(bordered, (0, 0), (w - 1, h - 1), color, thickness)
    return bordered


def create_black_image(height, width):
    """Create a black image of the specified size."""
    return np.zeros((height, width, 3), dtype=np.uint8)


def render_frame(training_folder, frame_number, experiments):
    """Render a frame with images and metrics for all experiments."""
    images_folder = os.path.join(training_folder, "images")
    lines_folder = os.path.join(training_folder, "validation_lines_2d")

    screenshot_file = os.path.join(images_folder, f"screenshot_{frame_number}.png")
    lines_file = os.path.join(lines_folder, f"frame_{frame_number}_lines.csv")

    # Load the screenshot image
    original = cv2.imread(screenshot_file)
    if original is None:
        print(f"Error: Could not load image {screenshot_file}")
        return None

    img_height, img_width = original.shape[:2]

    # Load validation lines
    validation_lines = None
    if os.path.exists(lines_file):
        validation_lines = np.loadtxt(lines_file, delimiter=',', ndmin=2)
        if validation_lines.size == 0:
            validation_lines = None

    num_validation = len(validation_lines) if validation_lines is not None and len(validation_lines) > 0 else 0

    # Create validation image (original with lines drawn)
    validation_image = original.copy()
    if validation_lines is not None and validation_lines.size > 0:
        for line in validation_lines:
            x1, y1, x2, y2 = int(line[0]), int(line[1]), int(line[2]), int(line[3])
            cv2.line(validation_image, (x1, y1), (x2, y2), (0, 255, 0), 2)

    # Build list of all images to display
    all_images = []

    # First: Original with red border
    original_labeled = add_label(original, "Original")
    original_bordered = add_border(original_labeled, (0, 0, 255))
    all_images.append(original_bordered)

    # Second: Validation with red border and line count
    validation_labeled = add_label(validation_image, "Validation")
    validation_bordered = add_border(validation_labeled, (0, 0, 255))
    # Add validation line count at bottom
    cv2.rectangle(validation_bordered, (0, img_height - 30), (img_width, img_height), (0, 0, 0), -1)
    cv2.putText(validation_bordered, f"Lines: {num_validation}", (10, img_height - 10),
                cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 0), 2)
    all_images.append(validation_bordered)

    # Run experiments and add metrics to each image
    for experiment in experiments:
        visualization, predicted_lines = experiment.detect_lines(original)
        num_predicted = len(predicted_lines) if predicted_lines is not None and len(predicted_lines) > 0 else 0

        labeled = add_label(visualization, experiment.name)
        labeled_with_metrics = add_metrics_to_image(labeled, num_predicted, num_validation)
        all_images.append(labeled_with_metrics)

    # Create rows with max IMAGES_PER_ROW images each
    rows = []
    for i in range(0, len(all_images), IMAGES_PER_ROW):
        row_images = all_images[i:i + IMAGES_PER_ROW]

        # Fill with black images if row has less than IMAGES_PER_ROW
        while len(row_images) < IMAGES_PER_ROW:
            row_images.append(create_black_image(img_height, img_width))

        row = np.hstack(row_images)
        rows.append(row)

    # Stack all rows vertically
    combined = np.vstack(rows)

    return combined


def create_experiments():
    """Create the list of experiments to run."""
    return [
        Experiment1(canny_low=50, canny_high=150, hough_threshold=50,
                    min_line_length=50, max_line_gap=10),
        Experiment2(sobel_ksize=3, threshold_value=50, hough_threshold=50,
                    min_line_length=50, max_line_gap=10),
    ]


def generate_and_save_predictions(training_folder, frames, experiments):
    """Generate predictions for all frames and save to predictions.json."""
    images_folder = os.path.join(training_folder, "images")

    # Build predictions structure
    predictions_data = {"experiments": []}

    for experiment in experiments:
        experiment_data = {
            "name": experiment.name,
            "predictions": []
        }

        for frame_number in frames:
            screenshot_file = os.path.join(images_folder, f"screenshot_{frame_number}.png")
            image = cv2.imread(screenshot_file)

            if image is None:
                continue

            _, predicted_lines = experiment.detect_lines(image)

            # Convert lines to list format for JSON
            lines_list = []
            if predicted_lines is not None and len(predicted_lines) > 0:
                lines_list = predicted_lines.tolist()

            experiment_data["predictions"].append({
                "image": f"screenshot_{frame_number}.png",
                "lines2d": lines_list
            })

        predictions_data["experiments"].append(experiment_data)

    # Save to JSON file
    predictions_file = os.path.join(training_folder, "predictions.json")
    with open(predictions_file, 'w') as f:
        json.dump(predictions_data, f, indent=2)

    print(f"Predictions saved to {predictions_file}")


def run_viewer(silent=False):
    """Main viewer loop for browsing training data."""
    training_folder = select_training_data_folder()
    if training_folder is None:
        return

    frames = get_available_frames(training_folder)
    if not frames:
        print("No frames available in selected training data folder")
        return

    # Create experiments
    experiments = create_experiments()

    # Generate and save predictions for all frames
    generate_and_save_predictions(training_folder, frames, experiments)

    # If silent mode, exit after generating predictions
    if silent:
        return

    current_index = 0
    window_name = "Line Detector 2D"

    print(f"Found {len(frames)} frames. Use LEFT/RIGHT arrow keys to navigate, ESC or 'q' to quit.")

    while True:
        frame_number = frames[current_index]
        image = render_frame(training_folder, frame_number, experiments)

        if image is None:
            break

        # Add frame info overlay at the very top
        cv2.putText(image, f"Frame {frame_number} ({current_index + 1}/{len(frames)})",
                    (10, 60), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 255), 2)

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
    parser = argparse.ArgumentParser(description='Line Detector 2D - Experiment viewer and prediction generator')
    parser.add_argument('--silent', action='store_true', default=False,
                        help='Only generate predictions.json without opening the viewer')
    args = parser.parse_args()

    run_viewer(silent=args.silent)
