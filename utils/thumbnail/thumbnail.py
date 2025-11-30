#!/usr/bin/env python3
# This script extracts a thumbnail image from a given screenshot.

import sys
import os
import cv2

def main():
    if len(sys.argv) != 3:
        print("Usage: python3 thumbnail.py <input_filepath> <output_filename>")
        sys.exit(1)

    input_path = sys.argv[1]
    output_dir = "../../thumbnails/"
    output_path = os.path.join(output_dir, sys.argv[2])

    os.makedirs(output_dir, exist_ok=True)

    img = cv2.imread(input_path)
    if img is None:
        print(f"Error: Failed to read image: {input_path}")
        sys.exit(1)

    # Define cropping parameters according to the output of check_pixels.cpp
    # (w_edge, h_edge) is the top-left corner of the square crop
    # size is the width and height of the square crop
    w_edge = 1141
    h_edge = 384
    size = 599
    thumbnail = img[h_edge:h_edge+size, w_edge:w_edge+size]
    quality_params = [cv2.IMWRITE_PNG_COMPRESSION, 0]

    if not cv2.imwrite(output_path, thumbnail, quality_params):
        print(f"Error: Failed to save thumbnail to {output_path}")
        sys.exit(1)

    print(f"Thumbnail saved to {output_path}")

if __name__ == "__main__":
    main()
