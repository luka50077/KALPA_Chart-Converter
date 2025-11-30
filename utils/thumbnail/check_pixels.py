import cv2
import sys

def on_mouse(event, x, y, flags, userdata):
    if event == cv2.EVENT_MOUSEMOVE:
        print(f"Mouse Position: ({x}, {y})")

def main():
    if len(sys.argv) < 2:
        print(f"Usage: python {sys.argv[0]} <image_path>")
        sys.exit(1)

    image_path = sys.argv[1]
    img = cv2.imread(image_path)

    if img is None:
        print("Failed to load image.")
        sys.exit(1)

    cv2.namedWindow("image", cv2.WINDOW_AUTOSIZE)
    cv2.setMouseCallback("image", on_mouse)

    while True:
        cv2.imshow("image", img)
        key = cv2.waitKey(10)
        if key == 27 or key == ord('q'):  # ESC or q
            break

    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
