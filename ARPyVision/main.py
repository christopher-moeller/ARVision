import os.path
import numpy as np
import cv2

INPUT_SERIES = "input/main_scene_20260130_115021"

def render_input(frame_number):
    print(f"Render frame {frame_number}")

    frame_lines_file = os.path.join(INPUT_SERIES, "output", f"frame_{frame_number}_lines.csv")
    screenshot_file = os.path.join(INPUT_SERIES, f"screenshot_{frame_number}.png")

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


if __name__ == '__main__':
    render_input(1)

