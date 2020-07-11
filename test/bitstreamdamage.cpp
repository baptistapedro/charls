// Copyright (c) Team CharLS.
// SPDX-License-Identifier: BSD-3-Clause

#include "bitstreamdamage.h"
#include "util.h"

#include <iostream>
#include <random>
#include <vector>

using charls::jpegls_errc;
using std::cout;
using std::mt19937;
using std::uniform_int_distribution;
using std::vector;

namespace {

void test_damaged_bit_stream1()
{
    vector<uint8_t> encodedBuffer = read_file("test/incorrect_images/InfiniteLoopFFMPEG.jls");

    vector<uint8_t> destination(256 * 256 * 2);
    const auto error = JpegLsDecode(destination.data(), destination.size(), encodedBuffer.data(), encodedBuffer.size(), nullptr, nullptr);
    assert::is_true(error == jpegls_errc::invalid_encoded_data);
}


void test_damaged_bit_stream2()
{
    vector<uint8_t> encodedBuffer = read_file("test/lena8b.jls");

    encodedBuffer.resize(900);
    encodedBuffer.resize(40000, 3);

    vector<uint8_t> destination(512 * 512);
    const auto error = JpegLsDecode(destination.data(), destination.size(), encodedBuffer.data(), encodedBuffer.size(), nullptr, nullptr);
    assert::is_true(error == jpegls_errc::invalid_encoded_data);
}


void test_damaged_bit_stream3()
{
    vector<uint8_t> encodedBuffer = read_file("test/lena8b.jls");

    encodedBuffer[300] = 0xFF;
    encodedBuffer[301] = 0xFF;

    vector<uint8_t> destination(512 * 512);
    const auto error = JpegLsDecode(destination.data(), destination.size(), encodedBuffer.data(), encodedBuffer.size(), nullptr, nullptr);
    assert::is_true(error == jpegls_errc::invalid_encoded_data);
}


void test_file_with_random_header_damage(const char* filename)
{
    const vector<uint8_t> encodedBufferOriginal = read_file(filename);

    mt19937 generator(102347325);

    MSVC_WARNING_SUPPRESS_NEXT_LINE(26496) // cannot be marked as const as operator() is not always defined const.
    MSVC_CONST uniform_int_distribution<uint32_t> distribution(0, 255);

    vector<uint8_t> destination(512 * 512);

    for (size_t i = 0; i < 40; ++i)
    {
        vector<uint8_t> encodedBuffer(encodedBufferOriginal);
        vector<int> errors(10, 0);

        for (int j = 0; j < 20; ++j)
        {
            encodedBuffer[i] = static_cast<uint8_t>(distribution(generator));
            encodedBuffer[i + 1] = static_cast<uint8_t>(distribution(generator));
            encodedBuffer[i + 2] = static_cast<uint8_t>(distribution(generator));
            encodedBuffer[i + 3] = static_cast<uint8_t>(distribution(generator));

            const auto error = JpegLsDecode(destination.data(), destination.size(), &encodedBuffer[0], encodedBuffer.size(), nullptr, nullptr);
            errors[static_cast<int>(error)]++;
        }

        cout << "With garbage input at index " << i << ": ";
        for (unsigned int error = 0; error < errors.size(); ++error)
        {
            if (errors[error] == 0)
                continue;

            cout << errors[error] << "x error (" << error << "); ";
        }

        cout << "\r\n";
    }
}


void test_random_malformed_header()
{
    test_file_with_random_header_damage("test/conformance/T8C0E0.JLS");
    test_file_with_random_header_damage("test/conformance/T8C1E0.JLS");
    test_file_with_random_header_damage("test/conformance/T8C2E0.JLS");
}


} // namespace


void damaged_bit_stream_tests()
{
    cout << "Test Damaged bit stream\r\n";
    test_damaged_bit_stream1();
    test_damaged_bit_stream2();
    test_damaged_bit_stream3();

    cout << "Begin random malformed bit stream tests:\n";
    test_random_malformed_header();
    cout << "End random malformed bit stream tests:\n";
}
