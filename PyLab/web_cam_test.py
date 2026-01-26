import cv2
import edge_transforms as et
import numpy as np

def open_webcam():
    # Open the default camera
    cam = cv2.VideoCapture(0)

    # Get the default frame width and height
    frame_width = int(cam.get(cv2.CAP_PROP_FRAME_WIDTH))
    frame_height = int(cam.get(cv2.CAP_PROP_FRAME_HEIGHT))

    # Define the codec and create VideoWriter object
    fourcc = cv2.VideoWriter_fourcc(*'mp4v')
    out = cv2.VideoWriter('output.mp4', fourcc, 20.0, (frame_width, frame_height))

    while True:
        ret, frame = cam.read()

        im = frame
        im = et.do_edge_detection(im)
        im = et.do_grey_edge_transformation(im)

        vectors = et.find_vectors_in_edges(im)

        blank_img = np.zeros_like(frame)

        im = et.draw_vectors_to_image(blank_img, vectors)

        compare_results = np.concatenate((im, frame), axis=1)

        # Write the frame to the output file
        out.write(im)

        # Display the captured frame
        cv2.imshow('Camera', compare_results)

        # Press 'q' to exit the loop
        if cv2.waitKey(1) == ord('q'):
            break

    # Release the capture and writer objects
    cam.release()
    out.release()
    cv2.destroyAllWindows()


if __name__ == "__main__":
    open_webcam()