import cv2
from ultralytics import YOLO

model = YOLO("yolo12n.pt")
print(model.names)
#webcamera = cv2.VideoCapture(0)
#webcamera = cv2.VideoCapture("20250319_231554000_iOS.MOV")
webcamera = cv2.VideoCapture("http://192.168.196.103:81/stream") #keep:81
#http://192.168.196.103/
# webcamera.set(cv2.CAP_PROP_FRAME_HEIGHT, 1080)

while True:
    success, frame = webcamera.read()
    
    results = model(frame, classes=0, conf=0.2, imgsz=640)
    cv2.putText(frame, f"Total: {len(results[0].boxes)}", (50, 50), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2, cv2.LINE_AA)
    cv2.imshow("Live Camera", results[0].plot())
        
    if cv2.waitKey(1) == ord('q'):
        break

webcamera.release()
cv2.destroyAllWindows()