import sys
import cv2
import os

target_resolution = (640, 360)
crop_borders = 0.23
fps = 90.

def main(root, outPath):
    
    fourcc = cv2.VideoWriter_fourcc(*'DIVX')
    writer = cv2.VideoWriter(outPath, fourcc, fps, target_resolution)
    
    images = sorted([os.path.join(root, img) for img in os.listdir(root)])
    
    size = len(images)
    
    for i, i_path in enumerate(images):
        img = cv2.imread(i_path)
        
        w, h, _ = img.shape
        
        if crop_borders:
            img = img[int(crop_borders*w):int(-crop_borders*w), int(crop_borders*h):int(-crop_borders*h),:]
        
        img = cv2.resize(img, target_resolution)
       
        writer.write(img)
        
        cv2.imshow("pp", img)
        
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
        
        if not i % 1000:
            print("Did {}/{} images ({:.2f}%)".format(i, size, 100 * i/size))
        
    writer.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    root_path = sys.argv[1]
    out_path = sys.argv[2]
    main(root_path, out_path)
