#include <iostream>
#include <vector>
#include <fstream>
#include <arm_neon.h>
#include <cstdint>
#include <string>
#include <chrono>
#include <dispatch/dispatch.h> // Apple's multithreading library

using namespace std;

uint64x2_t mandelbrot_neon(float64x2_t c_re,
                           float64x2_t c_im,
                           int maxIter)
{
    float64x2_t four = vdupq_n_f64(4.0);
    float64x2_t two  = vdupq_n_f64(2.0);
    uint64x2_t ones  = vdupq_n_u64(1);

    // Start z = c (same as your scalar function)
    float64x2_t z_re = c_re;
    float64x2_t z_im = c_im;

    uint64x2_t iters = vdupq_n_u64(0);

    for (int iter = 0; iter < maxIter; ++iter)
    {
        float64x2_t z_re2 = vmulq_f64(z_re, z_re);
        float64x2_t z_im2 = vmulq_f64(z_im, z_im);

        float64x2_t mag2 = vaddq_f64(z_re2, z_im2);

        uint64x2_t mask = vcleq_f64(mag2, four);

        // Both points escaped
        if (vgetq_lane_u64(mask, 0) == 0 &&
            vgetq_lane_u64(mask, 1) == 0)
        {
            break;
        }

        // Increment only active points
        iters = vaddq_u64(iters, vandq_u64(mask, ones));

        // z = z² + c
        float64x2_t z_im_new =
            vaddq_f64(vmulq_f64(two, vmulq_f64(z_re, z_im)), c_im);

        float64x2_t z_re_new =
            vaddq_f64(vsubq_f64(z_re2, z_im2), c_re);

        // Update only active lanes (mask & z_re_new) | (~mask & z_re)
        z_re = vreinterpretq_f64_u64(
                    vbslq_u64(mask,
                              vreinterpretq_u64_f64(z_re_new),
                              vreinterpretq_u64_f64(z_re)));

        // Update only active lanes (mask & z_im_new) | (~mask & z_im)
        z_im = vreinterpretq_f64_u64(
                    vbslq_u64(mask,
                              vreinterpretq_u64_f64(z_im_new),
                              vreinterpretq_u64_f64(z_im)));
    }

    return iters;
}


void calculate_mandelbrot(int width,
                          int height,
                          int maxIter,
                          double x_start,
                          double x_fin,
                          double y_start,
                          double y_fin,
                          std::vector<int>& buffer)
{
    double dx = (x_fin - x_start) / (width - 1);
    double dy = (y_fin - y_start) / (height - 1);

    dispatch_apply(height, DISPATCH_APPLY_AUTO, ^(size_t i)
    {
        double y = y_fin - i * dy;
        float64x2_t c_im = vdupq_n_f64(y);

        for (int j = 0; j < width; j += 2)
        {
            double c_re_arr[2] =
            {
                x_start + j * dx,
                x_start + (j + 1) * dx
            };

            float64x2_t c_re = vld1q_f64(c_re_arr);

            uint64x2_t iters = mandelbrot_neon(c_re, c_im, maxIter);

            uint64_t result[2];
            vst1q_u64(result, iters);

            buffer[i * width + j] = static_cast<int>(result[0]);

            if (j + 1 < width)
            {
                buffer[i * width + j + 1] =
                    static_cast<int>(result[1]);
            }
        }
    });
}


void write_image(const std::string& filename, int width, int height, int maxIter, const std::vector<int>& buffer) {
    std::ofstream img(filename, std::ios::binary);
    img << "P6\n" << width << " " << height << "\n255\n";

    std::vector<uint8_t> image_data(width * height * 3);

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            int value = buffer[i * width + j];
            int pixel_index = (i * width + j) * 3;

            if (value == maxIter) {
                image_data[pixel_index] = 0;
                image_data[pixel_index + 1] = 0;
                image_data[pixel_index + 2] = 0;
            } else {
                image_data[pixel_index]     = static_cast<uint8_t>(value % 256);
                image_data[pixel_index + 1] = static_cast<uint8_t>((value * 5) % 256);
                image_data[pixel_index + 2] = static_cast<uint8_t>((value * 10) % 256);
            }
        }
    }
    img.write(reinterpret_cast<const char*>(image_data.data()), image_data.size());
    img.close();
}

int main(int argc, char* argv[]) {
    // Default Values
    int width = 1920;
    int height = 1080;
    int maxIter = 1000;

    // Standard CLI Argument Parsing Loop
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-res_x" && i + 1 < argc) {
            width = std::stoi(argv[++i]);
        } else if (arg == "-res_y" && i + 1 < argc) {
            height = std::stoi(argv[++i]);
        } else if (arg == "-depth" && i + 1 < argc) {
            maxIter = std::stoi(argv[++i]);
        }
    }

    double x_start = 0.27, x_fin = 0.30;
    double y_start = 0.00, y_fin = 0.02;

    std::vector<int> mandelbrot_buffer(width * height);

    std::cout << "C++: Generating " << width << "x" << height << " image with depth " << maxIter << "...\n";
    
    auto t1 = std::chrono::high_resolution_clock::now();

    calculate_mandelbrot(width, height, maxIter, x_start, x_fin, y_start, y_fin, mandelbrot_buffer);
    
    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> calc_time = t2 - t1;
    std::cout << "Calculation finished in " << calc_time.count() << " seconds.\n";

    write_image("mandelbrot_cpp.ppm", width, height, maxIter, mandelbrot_buffer);

    auto t3 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> total_time = t3 - t1;
    std::cout << "Total time (including save): " << total_time.count() << " seconds.\n";

    return 0;
}