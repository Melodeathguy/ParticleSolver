import sys
import cv2
import os

resolution = (1280, 720)
fps = 90.

def main(root):
    
    fourcc = cv2.VideoWriter_fourcc(*'XVID')
    writer = cv2.VideoWriter('output.avi', fourcc, fps, resolution)
    
    images = sorted([os.path.join(root, img) for img in os.listdir(root)])
    
    for i_path in images:
        img = cv2.imread(i_path)
        
        img = cv2.resize(img, resolution)
        
        writer.write(img)
        
        cv2.imshow("pp", img)
        
        if cv2.waitKey(1) & 0xFF == ord('q'):
            writer.release()
            break
        
    writer.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    root_path = sys.argv[1]
    main(root_path)
