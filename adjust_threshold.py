import cv2
import numpy as np

h_lower = 53
s_lower = 132
v_lower = 103
h_upper = 151
s_upper = 189
v_upper = 146

# h_lower = 33
# s_lower = 146
# v_lower = 65
# h_upper = 177
# s_upper = 255
# v_upper = 130

# h_lower = 39
# s_lower = 90
# v_lower = 76
# h_upper = 93
# s_upper = 166
# v_upper = 178

# h_lower = 48
# s_lower = 89
# v_lower = 51
# h_upper = 96
# s_upper = 162
# v_upper = 178

def main():
    init_window()
    # cap = cv2.VideoCapture('/home/qff233/.local/share/rovhost/Videos/2023-03-24T17-07-23.199254+08.mkv')
    # cap = cv2.VideoCapture('v4l2src device=/dev/video0 ! video/x-raw, format=(string)YUY2, width=(int)640, height=(int)360 ! videoconvert ! appsink ')
    cap = cv2.VideoCapture('/home/qff233/.local/share/rovhost/Videos/1.mkv')
    global current_img
    n = 1
    while cap.isOpened():
        for _ in range(n):
            _, current_img = cap.read()
        update_show()
        key = cv2.waitKey(0)
        if key == 112:  # p3
            print_params()
            n = 0
        elif key == 113:
            break
        elif key == 115:
            n = 10
            continue
        else:
            n = 1
            continue


def process(frame):
    h, w, _ = frame.shape
    wh_prop = w / h
    frame = cv2.resize(frame, (int(480 * wh_prop), 480))
    img_hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2YCR_CB)
    h, w, _ = frame.shape
    mask = cv2.inRange(img_hsv, (h_lower, s_lower, v_lower), (h_upper, s_upper, v_upper))
    mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (h // 96, h // 96)))
    mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, cv2.getStructuringElement(cv2.MORPH_RECT, (h // 48, h // 48)))
    mask_img = cv2.cvtColor(mask, cv2.COLOR_GRAY2BGR)
    img = np.hstack((frame, mask_img))
    return img


def print_params():
    print(f'h_lower = {h_lower}')
    print(f's_lower = {s_lower}')
    print(f'v_lower = {v_lower}')
    print(f'h_upper = {h_upper}')
    print(f's_upper = {s_upper}')
    print(f'v_upper = {v_upper}')


def set_h_lower(x):
    global h_lower
    h_lower = x


def set_s_lower(x):
    global s_lower
    s_lower = x


def set_v_lower(x):
    global v_lower
    v_lower = x


def set_h_upper(x):
    global h_upper
    h_upper = x


def set_s_upper(x):
    global s_upper
    s_upper = x


def set_v_upper(x):
    global v_upper
    v_upper = x


def imupdate_wrapper(f):
    def wrapper(*args, **argw):
        f(*args, **argw)
        update_show()

    return wrapper


current_img = None


def update_show():
    if current_img is not None:
        frame = process(current_img)
        cv2.imshow(WIN_NAME, frame)


WIN_NAME = 'Preview'


def init_window():
    cv2.namedWindow(WIN_NAME)
    cv2.createTrackbar("Param1 Lower", WIN_NAME, h_lower, 255, imupdate_wrapper(set_h_lower))
    cv2.createTrackbar("Param2 Lower", WIN_NAME, s_lower, 255, imupdate_wrapper(set_s_lower))
    cv2.createTrackbar("Param3 Lower", WIN_NAME, v_lower, 255, imupdate_wrapper(set_v_lower))
    cv2.createTrackbar("Param1 Upper", WIN_NAME, h_upper, 255, imupdate_wrapper(set_h_upper))
    cv2.createTrackbar("Param2 Upper", WIN_NAME, s_upper, 255, imupdate_wrapper(set_s_upper))
    cv2.createTrackbar("Param3 Upper", WIN_NAME, v_upper, 255, imupdate_wrapper(set_v_upper))


if __name__ == '__main__':
    main()
