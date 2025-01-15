
#include <stdio.h>

#include <iostream>
#include <fstream>

#include <vector>
#include <array>

#include <cmath>
#include <string>

namespace zzwlib {
    namespace jpeg {
        void bmp2yuv444p(std::string bmp_file, std::string yuv_file) {
            std::ifstream bmp(bmp_file, std::ios::binary);
            std::ofstream yuv(yuv_file, std::ios::binary);
            if (!bmp.is_open()) {
                std::cout << "bmp file open failed" << std::endl;
                return;
            }
            if (!yuv.is_open()) {
                std::cout << "yuv file open failed" << std::endl;
                return;
            }
            std::vector<char> bmp_data(54);
            bmp.read(bmp_data.data(), 54);
            int width = *(int*)&bmp_data[18];
            int height = *(int*)&bmp_data[22];
            std::cout << "width: " << width << " height: " << height << std::endl;
            int padding = (4 - (width * 3) % 4) % 4;
            int bytes_per_row = (width * 3) + padding;

            std::cout << "padding: " << padding << " bytes_per_row: " << bytes_per_row << std::endl;

            std::vector<char> bmp_pixel_data(bytes_per_row * height);

            bmp.read(bmp_pixel_data.data(), bytes_per_row * height);

            std::vector<char> yuv_data(width * height * 3);
            char * y_plane = yuv_data.data();
            char * u_plane = yuv_data.data() + width * height;
            char * v_plane = yuv_data.data() + width * height * 2;
            for (int row = 0; row < height; row++) {
                for (int col = 0; col < width; col++) {
                    int bmp_index = (height - row - 1) * bytes_per_row + col * 3;
                    int r = bmp_pixel_data[bmp_index + 2];
                    int g = bmp_pixel_data[bmp_index + 1];
                    int b = bmp_pixel_data[bmp_index + 0];
                    int y = 0.299 * r + 0.587 * g + 0.114 * b;
                    int u = -0.169 * r - 0.331 * g + 0.5 * b + 128;
                    int v = 0.5 * r - 0.419 * g - 0.081 * b + 128;
                    y_plane[row * width + col] = y;
                    u_plane[row * width + col] = u;
                    v_plane[row * width + col] = v;
                }
            }
            yuv.write(yuv_data.data(), width * height * 3);
            yuv.close();
            bmp.close();
        }
    }
}

/*
 * ./bmp2yuv444.elf test.bmp test.227x198.yuv
 *  ffplay -f rawvideo -pixel_format yuv444p -video_size 16x16 test.yuv
 *
*/
int main(int argc, char** argv) {
    if (argc != 3) {
        std::cout << "usage: bmp2yuv444 bmp_file yuv_file" << std::endl;
        return 0;
    }
    std::string bmp_file = argv[1];
    std::string yuv_file = argv[2];
    zzwlib::jpeg::bmp2yuv444p(bmp_file, yuv_file);
    return 0;
}

