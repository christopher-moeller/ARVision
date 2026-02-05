import os
import glob
import numpy as np
import cv2
from abc import ABC, abstractmethod

TRAINING_DATA_DIR = "/Users/cmoeller/dev/projects/ARVision/ARPyVision/training_data"


class ImageProcessingStep(ABC):
    """Base class for image processing steps in the pipeline."""

    @property
    @abstractmethod
    def name(self) -> str:
        """Name of the processing step (displayed as label)."""
        pass

    @abstractmethod
    def process(self, image: np.ndarray) -> np.ndarray:
        """
        Process the input image and return the result.

        Args:
            image: Input image (BGR format from OpenCV)

        Returns:
            Processed image (BGR format)
        """
        pass


class GrayscaleStep(ImageProcessingStep):
    """Convert image to grayscale."""

    @property
    def name(self) -> str:
        return "Grayscale"

    def process(self, image: np.ndarray) -> np.ndarray:
        gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
        # Convert back to BGR for consistent display
        return cv2.cvtColor(gray, cv2.COLOR_GRAY2BGR)


class CannyEdgeStep(ImageProcessingStep):
    """Apply Canny edge detection."""

    def __init__(self, low_threshold=50, high_threshold=150):
        self.low_threshold = low_threshold
        self.high_threshold = high_threshold

    @property
    def name(self) -> str:
        return "Canny Edges"

    def process(self, image: np.ndarray) -> np.ndarray:
        # Convert to grayscale if needed
        if len(image.shape) == 3:
            gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
        else:
            gray = image
        edges = cv2.Canny(gray, self.low_threshold, self.high_threshold)
        # Convert back to BGR for consistent display
        return cv2.cvtColor(edges, cv2.COLOR_GRAY2BGR)


class HoughLinesStep(ImageProcessingStep):
    """Detect lines using Hough Line Transform."""

    def __init__(self, rho=1, theta=np.pi/180, threshold=50, min_line_length=50, max_line_gap=10):
        self.rho = rho
        self.theta = theta
        self.threshold = threshold
        self.min_line_length = min_line_length
        self.max_line_gap = max_line_gap

    @property
    def name(self) -> str:
        return "Hough Lines"

    def process(self, image: np.ndarray) -> np.ndarray:
        # Convert to grayscale if needed
        if len(image.shape) == 3:
            gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
        else:
            gray = image

        # Create output image (black background with green lines)
        output = np.zeros((image.shape[0], image.shape[1], 3), dtype=np.uint8)

        # Detect lines
        lines = cv2.HoughLinesP(
            gray,
            self.rho,
            self.theta,
            self.threshold,
            minLineLength=self.min_line_length,
            maxLineGap=self.max_line_gap
        )

        if lines is not None:
            for line in lines:
                x1, y1, x2, y2 = line[0]
                cv2.line(output, (x1, y1), (x2, y2), (0, 255, 0), 2)

        return output


class ImageProcessingPipeline:
    """Pipeline that chains multiple image processing steps."""

    def __init__(self):
        self.steps: list[ImageProcessingStep] = []

    def add_step(self, step: ImageProcessingStep):
        """Add a processing step to the pipeline."""
        self.steps.append(step)

    def process(self, image: np.ndarray) -> list[tuple[str, np.ndarray]]:
        """
        Run the pipeline and return all intermediate results.

        Returns:
            List of tuples (step_name, result_image)
        """
        results = []
        current_image = image.copy()

        for step in self.steps:
            current_image = step.process(current_image)
            results.append((step.name, current_image.copy()))

        return results


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


IMAGES_PER_ROW = 4
BORDER_THICKNESS = 10


def add_label(image, label):
    """Add a label to the top-left corner of an image."""
    labeled = image.copy()
    cv2.putText(labeled, label, (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 0), 2)
    return labeled


def add_border(image, color, thickness=BORDER_THICKNESS):
    """Add a colored border around an image."""
    bordered = image.copy()
    h, w = bordered.shape[:2]
    cv2.rectangle(bordered, (0, 0), (w - 1, h - 1), color, thickness)
    return bordered


def create_black_image(height, width):
    """Create a black image of the specified size."""
    return np.zeros((height, width, 3), dtype=np.uint8)


def render_frame(training_folder, frame_number, pipeline):
    """Render a frame with dynamic rows of images (max 4 per row)."""
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

    # Create validation image (original with lines drawn)
    validation_image = original.copy()
    if os.path.exists(lines_file):
        lines = np.loadtxt(lines_file, delimiter=',', ndmin=2)
        if lines.size > 0:
            for line in lines:
                x1, y1, x2, y2 = int(line[0]), int(line[1]), int(line[2]), int(line[3])
                cv2.line(validation_image, (x1, y1), (x2, y2), (0, 255, 0), 2)

    # Build list of all images to display
    all_images = []

    # First: Original with red border
    original_labeled = add_label(original, "Original")
    original_bordered = add_border(original_labeled, (0, 0, 255))  # Red in BGR
    all_images.append(original_bordered)

    # Second: Validation with red border
    validation_labeled = add_label(validation_image, "Validation")
    validation_bordered = add_border(validation_labeled, (0, 0, 255))  # Red in BGR
    all_images.append(validation_bordered)

    # Add pipeline images
    pipeline_results = pipeline.process(original)
    for step_name, result_image in pipeline_results:
        labeled = add_label(result_image, step_name)
        all_images.append(labeled)

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


def create_default_pipeline():
    """Create a default pipeline with example processing steps."""
    pipeline = ImageProcessingPipeline()
    pipeline.add_step(GrayscaleStep())
    pipeline.add_step(CannyEdgeStep(low_threshold=50, high_threshold=150))
    pipeline.add_step(HoughLinesStep(threshold=50, min_line_length=50, max_line_gap=10))
    return pipeline


def run_viewer():
    """Main viewer loop for browsing training data."""
    training_folder = select_training_data_folder()
    if training_folder is None:
        return

    frames = get_available_frames(training_folder)
    if not frames:
        print("No frames available in selected training data folder")
        return

    # Create the processing pipeline
    pipeline = create_default_pipeline()

    current_index = 0
    window_name = "Line Detector 2D"

    print(f"Found {len(frames)} frames. Use LEFT/RIGHT arrow keys to navigate, ESC or 'q' to quit.")

    while True:
        frame_number = frames[current_index]
        image = render_frame(training_folder, frame_number, pipeline)

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
    run_viewer()
