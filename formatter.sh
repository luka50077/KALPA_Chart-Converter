#!/bin/bash
cd "$(dirname "$0")"


while [ ! -f "converter/CMakeLists.txt" ] && [ "$PWD" != "/" ]; do
  cd ..
done

if [ ! -f "./converter/CMakeLists.txt" ]; then
  echo "Error: CMakeLists.txt not found. Please run this script inside the project tree."
  exit 1
fi

echo "Formatting sources under: $(pwd)"

find converter/src converter/include utils/frame_viewer utils/thumbnail \
  \( -name "*.cpp" -o -name "*.hpp" -o -name "*.c" -o -name "*.h" \) \
  -exec clang-format -i {} +

echo "Formatting complete."