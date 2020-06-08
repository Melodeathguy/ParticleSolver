import sys
import cv2
import os

resolution = (1680, 1050)

def main(root):
    
    fourcc = cv2.VideoWriter_fourcc(*'XVID')
    writer = cv2.VideoWriter('output.avi', fourcc, 100.0, resolution)
    
    images = sorted([os.path.join(root, img) for img in os.listdir(root)])
    
    for i_path in images:
        img = cv2.imread(i_path)
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
