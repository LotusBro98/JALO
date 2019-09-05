import cv2  as cv
import sys
import numpy as  np

model = cv.dnn.readNetFromCaffe("/home/alex/Soft/openpose/models/pose/coco/pose_deploy_linevec.prototxt", "/home/alex/Soft/openpose/models/pose/coco/pose_iter_440000.caffemodel")

video = cv.VideoCapture("../videos/Воровство в одном из магазинов.mp4")

distCoeffs = np.zeros((14,), np.float32)
camera_matrix = np.float32([
    [400, 0, 480],
    [0, 400, 360],
    [0, 0, 1]
])

points = np.float32([
    [300, 400],
    [400, 500],
    [500, 500],
    [400, 400],
])
M = np.eye(3, 3, dtype=np.float32)
r0 = np.zeros((3, 1), np.float32)

def mouseCallback(event, x, y, shit, bullshit):
    global M
    global r0

    if event != cv.EVENT_LBUTTONDOWN:
        return

    i = np.argmin(np.sum(np.square(points - (x, y)), axis=-1))
    points[i] = (x, y)

    ret, M = cv.solveP3P(np.float32([
        [0,0,0],
        [0,1,0],
        [1,0,0],
        [1,1,0],
    ])[:3],
    points[:3], camera_matrix, distCoeffs,cv.SOLVEPNP_P3P)
    print(M)
    M = cv.Rodrigues(M)[0]

cv.namedWindow("Image")
cv.setMouseCallback("Image", mouseCallback)

while True:
    ret, frame = video.read()
    if not ret:
        break

    for pt in points:
        cv.circle(frame, tuple(np.int32(pt)), 4, (0, 255, 0), 2)

    cv.polylines(frame, np.int32([points]), True, (0, 255, 0), 1)

    cv.imshow("Image", frame)
    cv.waitKey(20)

video.release()