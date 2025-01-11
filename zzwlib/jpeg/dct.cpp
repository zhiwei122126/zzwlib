
#include <cmath>
#include <array>
#include <iostream>
#include <stdio.h>

#include "../logger.hpp"

zzwlib::logger dct_logger("dct", zzwlib::loglevel::log_verbose_level);

namespace zzwlib {

    namespace jpeg {

        std::array<std::array<float,8>, 8> src_block;
        std::array<std::array<float,8>, 8> dct_block;
        void reset_dct_block() {
            for (int y = 0; y < 8; y++) {
                for (int x = 0; x < 8; x++) {
                    dct_block[y][x] = 0.0f;
                }
            }
        }
        void reset_src_block() {
            for (int y = 0; y < 8; y++) {
                for (int x = 0; x < 8; x++) {
                    src_block[y][x] = 0.0f;
                }
            }
        }
        void random_src_block() {
            for (int y = 0; y < 8; y++) {
                for (int x = 0; x < 8; x++) {
                    src_block[y][x] = rand() % 256 - 128;
                }
            }
        }
        void computeIDCT() {
            for (int y = 0; y < 8; y++) {
                for (int x = 0; x < 8; x++) {
                    float sum = 0.0f;
                    for (int u = 0; u < 8; u++) {
                        for (int v = 0; v < 8; v++) {
                            float alpha = (u == 0) ? 1.0f / std::sqrt(2.0f) : 1.0f;
                            float beta =  (v == 0) ? 1.0f / std::sqrt(2.0f) : 1.0f;
                            sum += alpha * beta * dct_block[u][v] * std::cos((2 * x + 1) * u * M_PI / 16.0) * std::cos((2 * y + 1) * v * M_PI / 16.0);
                        }
                    }
                    src_block[y][x] = sum / 4.0f;
                }
            }
        }

        void computeFDCT() {
            for (int u = 0; u < 8; u++) {
                for (int v = 0; v < 8; v++) {

                    float sum = 0.0f;
                    float alpha = (u == 0) ? 1.0f / std::sqrt(2.0f) : 1.0f;
                    float beta =  (v == 0) ? 1.0f / std::sqrt(2.0f) : 1.0f;

                    for (int y = 0; y < 8; y++) {
                        for (int x = 0; x < 8; x++) {
                            sum += src_block[y][x] * std::cos((2 * x + 1) * u * M_PI / 16.0) * std::cos((2 * y + 1) * v * M_PI / 16.0);
                        }
                    }
                    dct_block[u][v] = sum * alpha * beta / 4.0f;
                }
            }
        }

        void print_src_block()
        {
            for (int y = 0; y < 8; y++) {
                for (int x = 0; x < 8; x++) {
                    //LOGD(dct_logger, "src_block[%d][%d]=%f ", y, x, src_block[y][x]);
                    printf("%f ", src_block[y][x]);
                }
                //LOGD(dct_logger, "\n");
                printf("\n");
            }
        }

        void print_dct_block()
        {
            for (int y = 0; y < 8; y++) {
                for (int x = 0; x < 8; x++) {
                    //LOGD(dct_logger, "dct_block[%d][%d]=%f ", y, x, dct_block[y][x]);
                    printf("%f ", dct_block[y][x]);
                }
                //LOGD(dct_logger, "\n");
                printf("\n");
            }
        }
    }
}

int main(int argc, char *argv[])
{
    zzwlib::jpeg::reset_src_block();
    zzwlib::jpeg::reset_dct_block();

    zzwlib::jpeg::random_src_block();

    LOGD(dct_logger, "\n\nsrc_block:");
    zzwlib::jpeg::print_src_block();

    zzwlib::jpeg::computeFDCT();

    LOGD(dct_logger, "\n\ndct_block:");
    zzwlib::jpeg::print_dct_block();
 
    zzwlib::jpeg::reset_src_block();
    zzwlib::jpeg::computeIDCT();

    LOGD(dct_logger, "\n\nsrc_block:");
    zzwlib::jpeg::print_src_block();

    return 0;
}
