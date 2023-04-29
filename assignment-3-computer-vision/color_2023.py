import numpy as np
import cv2


def gstreamer_pipeline(
    capture_width=1280,
    capture_height=720,
    display_width=1280,
    display_height=720,
    framerate=30,
    flip_method=0,
):
    return (
        "nvarguscamerasrc ! "
        "video/x-raw(memory:NVMM), "
        "width=(int)%d, height=(int)%d, "
        "format=(string)NV12, framerate=(fraction)%d/1 ! "
        "nvvidconv flip-method=%d ! "
        "video/x-raw, width=(int)%d, height=(int)%d, format=(string)BGRx ! "
        "videoconvert ! "
        "video/x-raw, format=(string)BGR ! appsink max-buffers=1 drop=true"
        % (
            capture_width,
            capture_height,
            framerate,
            flip_method,
            display_width,
            display_height,
        )
    )


def gen_triangle_mask(rect_size):
    rect_size += 1
    begin = rect_size // 2 - 1
    end = rect_size // 2
    mask = []
    for i in range(rect_size):
        mask.append([1 if (begin < j <= end) else 0 for j in range(rect_size)])
        if i % 2 == 1:
            begin -= 1
            end += 1
    return np.array(mask)


def gen_square_mask(rect_size):
    rect_size += 1
    mask = []
    for i in range(rect_size):
        mask.append([1 for j in range(rect_size)])
    return np.array(mask)


def gen_rect_mask(rect_size):
    rect_size += 1
    begin = rect_size // 4
    end = begin * 3
    mask = []
    for i in range(rect_size):
        mask.append([1 if begin <= i <= end else 0 for j in range(rect_size)])
    return np.array(mask)


def invert_mask(mask):
    inverted = np.array(mask)
    inverted[inverted != 0] = 2
    inverted[inverted == 0] = 1
    inverted[inverted == 2] = 0
    return inverted


def match_form(image_mask, form_mask, rect_size):
    rect_size += 1
    inverted_image = invert_mask(image_mask)
    inverted_form = invert_mask(form_mask)
    match = (image_mask & form_mask) | (inverted_image & inverted_form)
    return np.count_nonzero(match) / (rect_size * rect_size)


def process(frame, rect_size, pos_x, pos_y, triangle_mask, square_mask, rectangle_mask):
    print(-1, square_mask)
    h_sensivity = 10
    s_h = 255
    v_h = 255
    s_l = 50
    v_l = 50
    DELTA = 10
    width, height, channels = frame.shape
    start_point = (int(height/2 - rect_size/2 + DELTA * pos_x), int(width/2 - rect_size/2 + DELTA * pos_y))
    end_point = (int(height/2 + rect_size/2 + DELTA * pos_x), int(width/2 + rect_size/2 + DELTA * pos_y))

    color = (255, 0, 0)
    thickness = 2
    rect = cv2.rectangle(frame, start_point, end_point, color, thickness)

    org = end_point
    font = cv2.FONT_HERSHEY_SIMPLEX
    fontScale = 0.7

    if start_point[0] < 0 or start_point[1] < 0 or end_point[0] >= height or end_point[1] >= width:
        text = cv2.putText(rect, "frame out of bounds", (10, 30), font, fontScale, color, thickness, cv2.LINE_AA)
        frame = text
        return frame

    grayscale = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)[start_point[1]:end_point[1] + 1, start_point[0]:end_point[0] + 1]
    _, gray_mask = cv2.threshold(grayscale, 127, 255, cv2.THRESH_BINARY)
    gray_mask = invert_mask(gray_mask)

    form_text = "shape not recognized"
    form_mask = square_mask
    threshold = 0.85
    if match_form(gray_mask, square_mask, rect_size) > threshold:
        form_text = "square"
        form_mask = square_mask
        threshold = match_form(gray_mask, square_mask, rect_size)
    elif match_form(gray_mask, rectangle_mask, rect_size) > threshold:
        form_text = "rectangle"
        form_mask = rectangle_mask
        threshold = match_form(gray_mask, rectangle_mask, rect_size)
    elif match_form(gray_mask, triangle_mask, rect_size) > threshold:
        form_text = "triangle"
        form_mask = triangle_mask
        threshold = match_form(gray_mask, triangle_mask, rect_size)
    else:
        text = cv2.putText(rect, form_text, (10, 30), font, fontScale, color, thickness, cv2.LINE_AA)
        frame = text
        return frame

    hsv_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    green_upper = np.array([60 + h_sensivity, s_h, v_h])
    green_lower = np.array([60 - h_sensivity, s_l, v_l])
    blue_upper = np.array([120 + h_sensivity, s_h, v_h])
    blue_lower = np.array([120 - h_sensivity, s_l, v_l])
    red_upper_1 = np.array([0 + h_sensivity, s_h, v_h])
    red_lower_1 = np.array([0, s_l, v_l])
    red_upper_2 = np.array([180, s_h, v_h])
    red_lower_2 = np.array([180 - h_sensivity, s_l, v_l])
    mask_frame = hsv_frame[start_point[1]:end_point[1] + 1, start_point[0]:end_point[0] + 1]
    mask_green = cv2.inRange(mask_frame, green_lower, green_upper)
    mask_blue = cv2.inRange(mask_frame, blue_lower, blue_upper)
    mask_red_1 = cv2.inRange(mask_frame, red_lower_1, red_upper_1)
    mask_red_2 = cv2.inRange(mask_frame, red_lower_2, red_upper_2)
    mask_red = np.array(mask_red_1) | np.array(mask_red_2)
    print(form_mask)
    green_rate = np.count_nonzero(mask_green & form_mask) / np.count_nonzero(form_mask)
    red_rate = np.count_nonzero(mask_red & form_mask) / np.count_nonzero(form_mask)
    blue_rate = np.count_nonzero(mask_blue & form_mask)/ np.count_nonzero(form_mask)
    print(green_rate, red_rate, blue_rate)

    color_text = "color not supported"
    text = form_text + ", " + color_text
    threshold *= 0.9
    if green_rate > threshold:
        color_text = "green"
        text = color_text + " " + form_text
    elif blue_rate > threshold:
        color_text = "blue"
        text = color_text + " " + form_text
    elif red_rate > threshold:
        color_text = "red"
        text = color_text + " " + form_text
    text = cv2.putText(rect, text, (10, 30), font, fontScale, color, thickness, cv2.LINE_AA)
    frame = text
    return frame


print('Press 4 to Quit the Application\n')

#Open Default Camera
cap = cv2.VideoCapture(0)

# frame positions
rect_size = 100
pos_x = 0
pos_y = 0

triangle_mask = gen_triangle_mask(rect_size)
rectangle_mask = gen_rect_mask(rect_size)
square_mask = gen_square_mask(rect_size)

while(cap.isOpened()):
    #Take each Frame
    ret, frame = cap.read()
    
    #Flip Video vertically (180 Degrees)
    frame = cv2.flip(frame, 180)

    invert = process(frame, rect_size, pos_x, pos_y, triangle_mask, square_mask, rectangle_mask)




    # Show video
    # cv2.imshow('Cam', frame)
    cv2.imshow('Inverted', invert)

    # Exit if "4" is pressed
    k = cv2.waitKey(1000) & 0xFF
    if k == 52 : #ord 4
        #Quit
        print ('Good Bye!')
        break
    if k == ord('w') or k == ord('W'): # W (up)
        pos_y -= 1
    if k == ord('a') or k == ord('A'): # A
        pos_x -= 1
    if k == ord('s') or k == ord('S'): # S
        pos_y += 1
    if k == ord('d') or k == ord('D'): # D
        pos_x += 1
    if k == ord('1'):
        rect_size -= 4
        if rect_size == 0:
            rect_size = 20
        triangle_mask = gen_triangle_mask(rect_size)
        rectangle_mask = gen_rect_mask(rect_size)
        square_mask = gen_square_mask(rect_size)
    if k == ord('2'):
        rect_size += 4
        triangle_mask = gen_triangle_mask(rect_size)
        rectangle_mask = gen_rect_mask(rect_size)
        square_mask = gen_square_mask(rect_size)

#Release the Cap and Video
cap.release()
cv2.destroyAllWindows()
