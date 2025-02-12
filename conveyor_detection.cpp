#include <opencv2/opencv.hpp>
#include <iostream>
#include <map>

using namespace cv;
using namespace std;

// Unique ID counter for tracked objects
int objectCounter = 0;

// Structure to store object information
struct TrackedObject {
    int id;
    Rect boundingBox;
    string shape;
    string size;
    bool fullyInside = false; // Tracks if object is fully inside the frame
};

// Map to store tracked objects (ID -> TrackedObject)
map<int, TrackedObject> trackedObjects;

/**
 * Classifies object size based on bounding box area.
 */
string classifySize(int area) {
    if (area >= 100 && area < 1000) return "Small";
    if (area >= 1000 && area < 3000) return "Medium";
    if (area >= 3000) return "Large";
    return "Unknown";
}

/**
 * Classifies object shape based on contour approximation.
 */
string classifyShape(vector<Point> contour, Rect boundingBox) {
    double peri = arcLength(contour, true);
    vector<Point> approx;
    approxPolyDP(contour, approx, 0.03 * peri, true);

    int vertices = approx.size();
    double aspectRatio = (double)boundingBox.width / boundingBox.height;

    if (vertices == 3) return "Triangle";
    if (vertices == 4) {
        return (aspectRatio >= 1.2 || aspectRatio <= 1/1.2) ? "Rectangle" : "Square";
    }
    return "Circle";
}

/**
 * Assigns or retrieves an object ID based on bounding box overlap.
 */
int getObjectID(Rect boundingBox) {
    for (auto &entry : trackedObjects) {
        Rect &existingBox = entry.second.boundingBox;
        if ((existingBox & boundingBox).area() > 0.5 * boundingBox.area()) {
            existingBox = boundingBox; // Update position
            return entry.first;
        }
    }
    return ++objectCounter; // Assign a new ID
}

int main() {
    cout << "Opening video..." << endl;

    VideoCapture cap("conveyor_belt_centered.mp4");
    if (!cap.isOpened()) {
        cerr << "Error: Cannot open video!" << endl;
        return -1;
    }

    cout << "Video opened successfully!" << endl;

    // Retrieve video properties
    int frameWidth = static_cast<int>(cap.get(CAP_PROP_FRAME_WIDTH));
    int frameHeight = static_cast<int>(cap.get(CAP_PROP_FRAME_HEIGHT));
    int fps = static_cast<int>(cap.get(CAP_PROP_FPS));

    // Initialize video writer
    VideoWriter videoWriter("output_detection.mp4",
                            VideoWriter::fourcc('m', 'p', '4', 'v'),
                            fps, Size(frameWidth, frameHeight));

    if (!videoWriter.isOpened()) {
        cerr << "Error: Could not open video writer!" << endl;
        return -1;
    }

    while (true) {
        Mat frame, gray, blurred, clahe_output, thresh;
        cap >> frame;
        if (frame.empty()) break;

        // Convert to grayscale
        cvtColor(frame, gray, COLOR_BGR2GRAY);

        // Apply CLAHE (Contrast Limited Adaptive Histogram Equalization)
        Ptr<CLAHE> clahe = createCLAHE(3.0);
        clahe->apply(gray, clahe_output);

        // Apply Gaussian Blur to reduce noise
        GaussianBlur(clahe_output, blurred, Size(5, 5), 0);

        // Apply adaptive thresholding for better contour detection
        adaptiveThreshold(blurred, thresh, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 11, 3);

        // Find object contours
        vector<vector<Point>> contours;
        vector<Vec4i> hierarchy;
        findContours(thresh, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        map<int, bool> activeIDs;

        for (const auto &contour : contours) {
            Rect boundingBox = boundingRect(contour);
            int objSize = boundingBox.area();

            if (objSize < 100) continue; // Ignore very small objects

            int objectID = getObjectID(boundingBox);

            // Ensure the object is fully inside before classification
            if (boundingBox.x > frameWidth * 0.1 && boundingBox.x + boundingBox.width < frameWidth * 0.9) {
                trackedObjects[objectID].fullyInside = true;
            }

            if (trackedObjects[objectID].fullyInside) {
                trackedObjects[objectID] = {objectID, boundingBox, classifyShape(contour, boundingBox), classifySize(objSize)};
                activeIDs[objectID] = true;

                // Draw bounding box and label
                Scalar color = Scalar(0, 255, 0);
                rectangle(frame, boundingBox, color, 2);
                string label = "ID: " + to_string(objectID) + " " + trackedObjects[objectID].shape + " - " + trackedObjects[objectID].size;
                putText(frame, label, Point(boundingBox.x, boundingBox.y - 10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 255), 2);
            }
        }

        // Remove objects that fully exit the frame
        for (auto it = trackedObjects.begin(); it != trackedObjects.end();) {
            if (it->second.boundingBox.x + it->second.boundingBox.width >= frameWidth) {
                it = trackedObjects.erase(it);
            } else {
                ++it;
            }
        }

        videoWriter.write(frame);
        imshow("Detected Objects", frame);
        if (waitKey(30) == 27) break;
    }

    cap.release();
    videoWriter.release();
    destroyAllWindows();

    cout << "Video saved as output_detection.mp4" << endl;
    return 0;
}
