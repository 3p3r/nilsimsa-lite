/*!
 * The MIT License (MIT)
 * =====================
 *
 * Copyright 2017 Sepehr Laal
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#ifndef _NILSIMSA_LITE_H_
#define _NILSIMSA_LITE_H_

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @fn nilsimsa_compute
 * @brief computes the Nilsimsa hash of the given data.
 * @arg data [IN] input data byte (char) array
 * @arg size [IN] input data size
 * @arg out [OUT] output hash string (must be allocated 
 * to hold exactly 64 characters + 1 null character. So
 * total of 65 characters e.g. char hash[65])
 */
void nilsimsa_compute(const char* data, int size, char* out);

/*!
 * @fn nilsimsa_compare
 * @brief compares two given Nilsimsa hashes
 * @returns A score between -127 and 128 where -127 means
 * completely uncorrelated data and 128 means same data.
 * @arg lhs [IN] left hand side hash
 * @arg rhs [IN] right hand side hash
 */
int nilsimsa_compare(const char* lhs, const char* rhs);

#ifdef __cplusplus
} //!extern "C"
#endif

#endif //!_NILSIMSA_LITE_H_
