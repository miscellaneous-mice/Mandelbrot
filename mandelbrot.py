import argparse
import numpy as np
from numba import njit, prange
import time

@njit(parallel=True, fastmath=True)
def calculate_mandelbrot(width, height, max_iter, x_start, x_fin, y_start, y_fin):
    iters = np.zeros((height, width), dtype=np.int32)
    
    dx = (x_fin - x_start) / (width - 1)
    dy = (y_fin - y_start) / (height - 1)
    
    for i in prange(height):
        y = y_fin - i * dy
        
        for j in range(width):
            x = x_start + j * dx
            
            z_re = x
            z_im = y
            
            for iter_count in range(max_iter):
                z_re2 = z_re * z_re
                z_im2 = z_im * z_im
                
                if z_re2 + z_im2 > 4.0:
                    iters[i, j] = iter_count
                    break
                
                z_im = 2.0 * z_re * z_im + y
                z_re = z_re2 - z_im2 + x
            else:
                iters[i, j] = max_iter
                
    return iters

def write_image(filename, iters, max_iter):
    height, width = iters.shape
    image_data = np.zeros((height, width, 3), dtype=np.uint8)
    
    escaped_mask = (iters < max_iter)
    escaped_iters = iters[escaped_mask]
    
    image_data[escaped_mask, 0] = escaped_iters % 256
    image_data[escaped_mask, 1] = (escaped_iters * 5) % 256
    image_data[escaped_mask, 2] = (escaped_iters * 10) % 256
    
    with open(filename, 'wb') as f:
        header = f"P6\n{width} {height}\n255\n"
        f.write(header.encode('ascii'))
        f.write(image_data.tobytes())

def main():
    # Setup Command Line Argument Parsing
    parser = argparse.ArgumentParser(description="Multithreaded Numpy Mandelbrot")
    parser.add_argument('-res_x', type=int, default=1920, help="Image Width")
    parser.add_argument('-res_y', type=int, default=1080, help="Image Height")
    parser.add_argument('-depth', type=int, default=1000, help="Max Iterations")
    args = parser.parse_args()

    width = args.res_x
    height = args.res_y
    max_iter = args.depth

    # Zoomed into "Elephant Valley" for cool details
    x_start, x_fin = 0.27, 0.30
    y_start, y_fin = 0.00, 0.02

    print(f"Python: Generating {width}x{height} image with depth {max_iter}...")
    start_time = time.time()
    
    iters_buffer = calculate_mandelbrot(width, height, max_iter, x_start, x_fin, y_start, y_fin)
    
    calc_time = time.time()
    print(f"Calculation finished in {calc_time - start_time:.4f} seconds.")
    
    write_image("mandelbrot_python.ppm", iters_buffer, max_iter)
    
    print(f"Total time (including save): {time.time() - start_time:.4f} seconds.")

if __name__ == '__main__':
    main()