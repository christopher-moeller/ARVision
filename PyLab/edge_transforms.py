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

def line_features(lines):
    """
    lines: (N, 4) array of x1,y1,x2,y2
    returns: angles, midpoints, lengths
    """
    dx = lines[:, 2] - lines[:, 0]
    dy = lines[:, 3] - lines[:, 1]

    angles = np.arctan2(dy, dx)  # in radians
    lengths = np.hypot(dx, dy)

    mx = (lines[:, 0] + lines[:, 2]) / 2
    my = (lines[:, 1] + lines[:, 3]) / 2
    midpoints = np.stack([mx, my], axis=1)

    return angles, midpoints, lengths

def reduce_similar_lines(lines, angle_thresh=np.deg2rad(5), dist_thresh=20):
    """
    lines: (N, 4) array
    returns: reduced set of lines
    """
    angles, midpoints, lengths = line_features(lines)

    used = np.zeros(len(lines), dtype=bool)
    reduced = []

    for i in range(len(lines)):
        if used[i]:
            continue

        # Start a new cluster with line i
        cluster = [i]
        used[i] = True

        for j in range(i + 1, len(lines)):
            if used[j]:
                continue

            # Angle similarity
            if abs(angles[i] - angles[j]) > angle_thresh:
                continue

            # Spatial proximity
            dist = np.linalg.norm(midpoints[i] - midpoints[j])
            if dist > dist_thresh:
                continue

            cluster.append(j)
            used[j] = True

        # Keep the longest line in this cluster
        cluster_lengths = lengths[cluster]
        best_idx = cluster[np.argmax(cluster_lengths)]
        reduced.append(lines[best_idx])

    return np.array(reduced)


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

    # Reduce similar ones
    lines = reduce_similar_lines(
        lines,
        angle_thresh=np.deg2rad(5),  # allow 5° difference
        dist_thresh=30  # allow 30px midpoint distance
    )

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