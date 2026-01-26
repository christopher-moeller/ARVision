import cv2
import numpy as np
import edge_transforms as et


if __name__ == "__main__":
    original = cv2.imread("assets/arv-test.png", cv2.IMREAD_UNCHANGED)
    im = original
    im = et.do_edge_detection(im)
    im = et.do_grey_edge_transformation(im)

    vectors = et.find_vectors_in_edges(im)
    print(f"Found {len(vectors)} lines")

    blank_img = np.zeros_like(original)

    im = et.draw_vectors_to_image(blank_img, vectors)

    compare_results = np.concatenate((im, original), axis=1)
    cv2.imshow("Edge Detection", compare_results)

    cv2.waitKey(0)
    cv2.destroyAllWindows()
