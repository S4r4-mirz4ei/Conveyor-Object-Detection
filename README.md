# Conveyor Belt Object Detection & Tracking

This project implements **real-time object detection and tracking** on a simulated conveyor belt using **C++ and OpenCV**. The system detects objects of different shapes, sizes, and colors, assigns unique IDs, and tracks them until they exit the frame.

## Features
- **Real-time object detection** using OpenCV
- **Shape classification** (Circle, Square, Rectangle)
- **Size classification** (Small, Medium, Large)
- **Unique object tracking** with consistent ID assignment
- **Adaptive thresholding & contrast enhancement** for improved detection
- **Video processing & output generation**

## Installation
### Prerequisites
- C++17 or later
- OpenCV 4.x
- Git (for cloning)

### Setup
1. **Clone the repository**:
   ```
   git clone https://github.com/S4r4-mirz4ei/Conveyor-Object-Detection.git
   cd Conveyor-Object-Detection
   ```
2. **Compile the C++ code:**
```
   g++ conveyor_detection.cpp -o conveyor_detection `pkg-config --cflags --libs opencv4` -std=c++17
```
3. **Run the program:**
```
./conveyor_detection
```
## Input & Output
- Input: conveyor_belt_centered.mp4 (Simulated conveyor belt video)
- Output: output_detection.mp4 (Video with detected objects and assigned IDs)

---

 **Author**
👤 Sara Mirzaei
📧 Email: sara.mirz4ei@gmail.com
🔗 GitHub: [S4r4-mirz4ei](https://github.com/S4r4-mirz4ei)
