import cv2
import numpy as np

def do_edge_detection(img):
    sobelx = cv2.Sobel(img, cv2.CV_64F, 1, 0, ksize=3)  # Horizontal edges
    sobely = cv2.Sobel(img, cv2.CV_64F, 0, 1, ksize=3)  # Vertical edges

    # Compute gradient magnitude
    gradient_magnitude = cv2.magnitude(sobelx, sobely)

    # Convert to uint8
    gradient_magnitude = cv2.convertScaleAbs(gradient_magnitude)
    return gradient_magnitude

def do_grey_edge_transformation(im):
    if len(im.shape) == 3:
        gray = cv2.cvtColor(im, cv2.COLOR_BGR2GRAY)
    else:
        gray = im

    # Blur
    blurred = cv2.GaussianBlur(gray, (5, 5), 0)

    # Canny edges
    edges = cv2.Canny(blurred, threshold1=50, threshold2=150)
    return edges

def sort_lines(lines):
    # lines must be shape (N, 4)
    dx = lines[:, 2] - lines[:, 0]
    dy = lines[:, 3] - lines[:, 1]
    lengths = np.hypot(dx, dy)

    order = np.argsort(-lengths)  # descending
    lines_sorted = lines[order]
    return lines_sorted

def find_vectors_in_edges(im):
    lines = cv2.HoughLinesP(
        im,
        rho=1,  # distance resolution in pixels
        theta=np.pi / 180,  # angle resolution in radians
        threshold=100,  # minimum votes
        minLineLength=50,  # minimum length of line
        maxLineGap=20  # maximum gap between line segments
    )

    # This will hold your list of line vectors
    line_vectors = []

    if lines is None:
        return line_vectors

    # FIX: reshape from (N, 1, 4) to (N, 4)
    lines = lines.reshape(-1, 4)

    lines = sort_lines(lines)
    lines = lines[:50]
    for x1, y1, x2, y2 in lines:
        line_vectors.append(((x1, y1), (x2, y2)))

    return line_vectors

def draw_vectors_to_image(im, vectors):

    if len(im.shape) == 2:
        color_img = cv2.cvtColor(im, cv2.COLOR_GRAY2BGR)
    else:
        color_img = im.copy()

    for line in vectors:
        cv2.line(color_img, line[0], line[1], (0, 0, 255), 2)

    return color_img