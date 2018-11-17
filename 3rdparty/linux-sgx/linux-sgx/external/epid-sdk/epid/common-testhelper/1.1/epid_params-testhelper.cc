/*############################################################################
  # Copyright 2016-2017 Intel Corporation
  #
  # Licensed under the Apache License, Version 2.0 (the "License");
  # you may not use this file except in compliance with the License.
  # You may obtain a copy of the License at
  #
  #     http://www.apache.org/licenses/LICENSE-2.0
  #
  # Unless required by applicable law or agreed to in writing, software
  # distributed under the License is distributed on an "AS IS" BASIS,
  # WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  # See the License for the specific language governing permissions and
  # limitations under the License.
  ############################################################################*/

/*!
 * \file
 * \brief Intel(R) EPID 1.1 parameters C++ wrapper implementation.
 */

#include <cstring>

#include "epid/common-testhelper/1.1/epid_params-testhelper.h"
#include "epid/common-testhelper/bignum_wrapper-testhelper.h"
#include "epid/common-testhelper/errors-testhelper.h"
#include "epid/common-testhelper/ffelement_wrapper-testhelper.h"
#include "epid/common-testhelper/finite_field_wrapper-testhelper.h"

extern "C" {
#include "epid/common/math/src/ecgroup-internal.h"
#include "epid/common/math/src/finitefield-internal.h"
#include "epid/common/math/src/pairing-internal.h"
}

#ifndef COUNT_OF
#define COUNT_OF(a) (sizeof(a) / sizeof((a)[0]))
#endif  // COUNT_OF

Epid11ParamsObj::Epid11ParamsObj() {
  // construct Fq finite field
  FiniteFieldObj fq(this->q_str_);

  // Create G1
  // G1 is an elliptic curve group E(Fq).It can be initialized as follows:
  // Set G1 = E(Fq).init(p, q, h, a, b, g1.x, g1.y)
  G1 = EcGroupObj(&fq, FfElementObj(&fq, a_str_), FfElementObj(&fq, b_str_),
                  FfElementObj(&fq, g1_str_.x), FfElementObj(&fq, g1_str_.y),
                  BigNumObj(p_str_), BigNumObj(h_str_));

  // construct Fq1 finite field
  FiniteFieldObj fq1(this->q1_str_);

  // Create G3
  // G3 is an elliptic curve group E(Fq) as well (but with different
  // parameters).
  // It can be initialized as follows:
  // Set G3 = Fq.init(p', q', h', a', b', g3.x, g3.y)
  G3 =
      EcGroupObj(&fq1, FfElementObj(&fq1, a1_str_), FfElementObj(&fq1, b1_str_),
                 FfElementObj(&fq1, g3_str_.x), FfElementObj(&fq1, g3_str_.y),
                 BigNumObj(p1_str_), BigNumObj(h1_str_));

  // construct Fqd finite field
  FiniteFieldObj fqd(fq, this->coeffs_str_, COUNT_OF(this->coeffs_str_));

  // Fqk ground element is {-qnr, 0, 0}
  FfElementObj neg_qnr(&fq);
  THROW_ON_EPIDERR(FfNeg(fq, FfElementObj(&fq, qnr_str), neg_qnr));
  Fq3ElemStr ground_element_str = {0};
  THROW_ON_EPIDERR(WriteFfElement(fq, neg_qnr, &ground_element_str.a[0],
                                  sizeof(ground_element_str.a[0])));
  FfElementObj ground_element(&fqd, ground_element_str);

  // construct GT:=Fqk finite field
  GT = FiniteFieldObj(fqd, ground_element, 2);

  // G2 is an elliptic curve group E(Fqd).It can be initialized as follows:

  FfElementObj qnr(&fq, qnr_str);

  // twista = (a * qnr * qnr) mod q
  FfElementObj twista_fq(&fq, a_str_);
  THROW_ON_EPIDERR(FfMul(fq, twista_fq, qnr, twista_fq));
  THROW_ON_EPIDERR(FfMul(fq, twista_fq, qnr, twista_fq));
  Fq3ElemStr twista_str = {0};
  THROW_ON_EPIDERR(
      WriteFfElement(fq, twista_fq, &twista_str.a[0], sizeof(twista_str.a[0])));

  // twistb = (b * qnr * qnr * qnr) mod q
  FfElementObj twistb_fq(&fq, b_str_);
  THROW_ON_EPIDERR(FfMul(fq, twistb_fq, qnr, twistb_fq));
  THROW_ON_EPIDERR(FfMul(fq, twistb_fq, qnr, twistb_fq));
  THROW_ON_EPIDERR(FfMul(fq, twistb_fq, qnr, twistb_fq));
  Fq3ElemStr twistb_str = {0};
  THROW_ON_EPIDERR(
      WriteFfElement(fq, twistb_fq, &twistb_str.a[0], sizeof(twistb_str.a[0])));

  // cofactor is 1 for G2
  const BigNumStr h2_str_ = {
      {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}}};

  // Create G2
  // Set G2 = E(Fqd).init(orderG2, param(Fqd), twista, twistb, g2.x, g2.y).
  G2 = EcGroupObj(&fqd, FfElementObj(&fqd, twista_str),
                  FfElementObj(&fqd, twistb_str), FfElementObj(&fqd, g2x_str_),
                  FfElementObj(&fqd, g2y_str_), BigNumObj(orderG2_str),
                  BigNumObj(h2_str_));
}

const BigNumStr Epid11ParamsObj::p_str_ = {
    {{0x00, 0x00, 0x89, 0x57, 0x3F, 0x17, 0x47, 0x30, 0x8C, 0x43, 0xD5,
      0xEE, 0x41, 0x97, 0x96, 0x19, 0x72, 0xBB, 0x86, 0x88, 0xED, 0x4B,
      0xEF, 0x04, 0xAB, 0xAE, 0xC3, 0x8E, 0xEC, 0x51, 0xC3, 0xD3}}};

const BigNumStr Epid11ParamsObj::q_str_ = {
    {{0x09, 0xF9, 0x24, 0xE5, 0xD9, 0xBC, 0x67, 0x7F, 0x81, 0x0D, 0xF0,
      0x25, 0x58, 0xF7, 0x53, 0x13, 0xA9, 0x8A, 0xA6, 0x10, 0x47, 0x65,
      0x5D, 0x73, 0x9E, 0xF1, 0x94, 0xEB, 0x05, 0xB1, 0xA7, 0x11}}};

const std::vector<uint8_t> Epid11ParamsObj::h_str_ = {0x00, 0x00, 0x12, 0x97};
const FqElemStr Epid11ParamsObj::a_str_ = {
    {{0x05, 0x53, 0xD7, 0xC8, 0x81, 0xF7, 0x78, 0xC2, 0x2C, 0x37, 0xB6,
      0xC0, 0x16, 0x3E, 0x68, 0x24, 0x3A, 0x84, 0x78, 0x1C, 0x0A, 0xDF,
      0x9B, 0xB3, 0xED, 0x21, 0xC4, 0x46, 0xE5, 0xA7, 0xA3, 0x92}}};
const FqElemStr Epid11ParamsObj::b_str_ = {
    {{0x00, 0x3A, 0x2E, 0x39, 0x0E, 0x10, 0xD8, 0xAC, 0x47, 0xCB, 0x29,
      0xC8, 0xF1, 0x2C, 0x7F, 0x11, 0x99, 0x2A, 0x18, 0xB7, 0xEF, 0x73,
      0x48, 0xA6, 0xBE, 0x70, 0xA6, 0x8B, 0x97, 0x34, 0x8A, 0xB1}}};

const BigNumStr Epid11ParamsObj::coeffs_str_[3] = {
    {{{0x02, 0x16, 0x7A, 0x61, 0x53, 0xDD, 0xF6, 0xE2, 0x89, 0x15, 0xA0,
       0x94, 0xF1, 0xB5, 0xDC, 0x65, 0x21, 0x15, 0x62, 0xE1, 0x7D, 0xC5,
       0x43, 0x89, 0xEE, 0xB4, 0xEF, 0xC8, 0xA0, 0x8E, 0x34, 0x0F}}},

    {{{0x04, 0x82, 0x27, 0xE1, 0xEB, 0x98, 0x64, 0xC2, 0x8D, 0x8F, 0xDD,
       0x0E, 0x82, 0x40, 0xAE, 0xD4, 0x31, 0x63, 0xD6, 0x46, 0x32, 0x16,
       0x85, 0x7A, 0xB7, 0x18, 0x68, 0xB8, 0x17, 0x02, 0x81, 0xA6}}},

    {{{0x06, 0x20, 0x76, 0xE8, 0x54, 0x54, 0x53, 0xB4, 0xA9, 0xD8, 0x44,
       0x4B, 0xAA, 0xFB, 0x1C, 0xFD, 0xAE, 0x15, 0xCA, 0x29, 0x79, 0xA6,
       0x24, 0xA4, 0x0A, 0xF6, 0x1E, 0xAC, 0xED, 0xFB, 0x10, 0x41}}}};

const FqElemStr Epid11ParamsObj::qnr_str = {
    {0x08, 0x66, 0xA7, 0x67, 0x36, 0x6E, 0x62, 0x71, 0xB7, 0xA6, 0x52,
     0x94, 0x8F, 0xFB, 0x25, 0x9E, 0xE6, 0x4F, 0x25, 0xE5, 0x26, 0x9A,
     0x2B, 0x6E, 0x7E, 0xF8, 0xA6, 0x39, 0xAE, 0x46, 0xAA, 0x24}};

const std::vector<uint8_t> Epid11ParamsObj::orderG2_str = {
    0x00, 0x03, 0xDF, 0xFC, 0xBE, 0x2F, 0x5C, 0x2E, 0x45, 0x49, 0x7A, 0x2A,
    0x91, 0xBA, 0xD1, 0x3E, 0x01, 0xEC, 0x5F, 0xC2, 0x15, 0x14, 0x10, 0xB3,
    0x28, 0x5E, 0x56, 0xCC, 0x26, 0x51, 0x24, 0x93, 0x0E, 0x6C, 0x99, 0x96,
    0x38, 0xE0, 0x7D, 0x68, 0x8C, 0xB7, 0x97, 0x23, 0xF4, 0xAC, 0x4D, 0xBC,
    0x5E, 0x01, 0x15, 0xFF, 0x45, 0x60, 0x08, 0x13, 0xCD, 0x59, 0xD7, 0x73,
    0xB0, 0x0C, 0x20, 0x5E, 0xAB, 0xAA, 0x24, 0x31, 0xE2, 0x2A, 0xA2, 0x53,
    0x8A, 0xF7, 0x86, 0xD5, 0x19, 0x78, 0xC5, 0x55, 0x9C, 0x08, 0xB7, 0xE2,
    0xF4, 0xD0, 0x37, 0x74, 0x93, 0x56, 0x62, 0x7B, 0x95, 0xCC, 0x2C, 0xB0};

const BigNumStr Epid11ParamsObj::p1_str_ = {
    {{0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xBC, 0xE6, 0xFA, 0xAD, 0xA7, 0x17,
      0x9E, 0x84, 0xF3, 0xB9, 0xCA, 0xC2, 0xFC, 0x63, 0x25, 0x51}}};

const BigNumStr Epid11ParamsObj::q1_str_ = {
    {{0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}}};

const std::vector<uint8_t> Epid11ParamsObj::h1_str_ = {0x00, 0x00, 0x00, 0x01};

const FqElemStr Epid11ParamsObj::a1_str_ = {
    {{0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC}}};
const FqElemStr Epid11ParamsObj::b1_str_ = {
    {{0x5A, 0xC6, 0x35, 0xD8, 0xAA, 0x3A, 0x93, 0xE7, 0xB3, 0xEB, 0xBD,
      0x55, 0x76, 0x98, 0x86, 0xBC, 0x65, 0x1D, 0x06, 0xB0, 0xCC, 0x53,
      0xB0, 0xF6, 0x3B, 0xCE, 0x3C, 0x3E, 0x27, 0xD2, 0x60, 0x4B}}};

const Epid11G1ElemStr Epid11ParamsObj::g1_str_ = {
    {{{0x07, 0x78, 0x3B, 0x0D, 0xFE, 0x4A, 0xA3, 0x19, 0x49, 0xB0, 0xCE,
       0xAF, 0x3F, 0x74, 0x0F, 0x32, 0x16, 0x0C, 0x8B, 0x46, 0x94, 0x5B,
       0xA5, 0xB0, 0xE4, 0x8A, 0xDA, 0xD8, 0x88, 0x32, 0x90, 0x53}}},
    {{{0x08, 0xF7, 0xA2, 0xAA, 0xBA, 0x62, 0xB3, 0xFE, 0x29, 0x80, 0xC9,
       0x5B, 0x63, 0x53, 0xC8, 0x24, 0x3C, 0x7C, 0x1F, 0x4C, 0xDA, 0xCD,
       0xE5, 0x5F, 0xA2, 0x36, 0x93, 0x04, 0x3C, 0x3A, 0xBC, 0x2E}}}};
const Fq3ElemStr Epid11ParamsObj::g2x_str_ = {
    {{{{0x02, 0x10, 0x9A, 0xF4, 0x06, 0x32, 0x30, 0x89, 0xCB, 0x95, 0xE9,
        0x55, 0x0E, 0x9D, 0xAF, 0x0E, 0x98, 0xCD, 0xCA, 0xDC, 0xB1, 0xFF,
        0xFC, 0xD1, 0x45, 0x66, 0xBB, 0x86, 0x46, 0x1E, 0x8C, 0x30}}},
     {{{0x04, 0x78, 0x53, 0xE1, 0x3F, 0x96, 0xC5, 0xE4, 0x15, 0x23, 0x7B,
        0x1F, 0x3F, 0x2C, 0xD3, 0x95, 0x40, 0xBC, 0x7A, 0x31, 0x1F, 0x14,
        0x38, 0x9E, 0x1A, 0xA5, 0xD6, 0x63, 0x10, 0x91, 0xE4, 0xD3}}},
     {{{0x00, 0xB4, 0x02, 0xBC, 0x47, 0xFA, 0xA6, 0x29, 0x82, 0x0B, 0xB1,
        0xD5, 0xFF, 0xF2, 0xE6, 0xB0, 0xC6, 0xAE, 0xE8, 0x7B, 0x91, 0xD9,
        0xEE, 0x66, 0x07, 0x1F, 0xFD, 0xA2, 0xE7, 0x02, 0x66, 0xDD}}}}};

const Fq3ElemStr Epid11ParamsObj::g2y_str_ = {
    {{{{0x05, 0x2E, 0xF8, 0xC6, 0xC1, 0x6A, 0xEF, 0x3C, 0xC1, 0x95, 0xF6,
        0x26, 0xCE, 0x5E, 0x55, 0xD1, 0x64, 0x13, 0x28, 0xB1, 0x18, 0x57,
        0xD8, 0x1B, 0x84, 0xFA, 0xEC, 0x7E, 0x5D, 0x99, 0x06, 0x49}}},
     {{{0x05, 0x73, 0x35, 0xA9, 0xA7, 0xF2, 0xA1, 0x92, 0x5F, 0x3E, 0x7C,
        0xDF, 0xAC, 0xFE, 0x0F, 0xF5, 0x08, 0xD0, 0x3C, 0xAE, 0xCD, 0x58,
        0x00, 0x5F, 0xD0, 0x84, 0x7E, 0xEA, 0x63, 0x57, 0xFE, 0xC6}}},
     {{{0x01, 0x56, 0xDA, 0xF3, 0x72, 0x61, 0xDA, 0xC6, 0x93, 0xB0, 0xAC,
        0xEF, 0xAA, 0xD4, 0x51, 0x6D, 0xCA, 0x71, 0x1E, 0x06, 0x73, 0xEA,
        0x83, 0xB2, 0xB1, 0x99, 0x4A, 0x4D, 0x4A, 0x0D, 0x35, 0x07}}}}};
const Epid11G3ElemStr Epid11ParamsObj::g3_str_ = {
    {{{0x6B, 0x17, 0xD1, 0xF2, 0xE1, 0x2C, 0x42, 0x47, 0xF8, 0xBC, 0xE6,
       0xE5, 0x63, 0xA4, 0x40, 0xF2, 0x77, 0x03, 0x7D, 0x81, 0x2D, 0xEB,
       0x33, 0xA0, 0xF4, 0xA1, 0x39, 0x45, 0xD8, 0x98, 0xC2, 0x96}}},
    {{{0x4F, 0xE3, 0x42, 0xE2, 0xFE, 0x1A, 0x7F, 0x9B, 0x8E, 0xE7, 0xEB,
       0x4A, 0x7C, 0x0F, 0x9E, 0x16, 0x2B, 0xCE, 0x33, 0x57, 0x6B, 0x31,
       0x5E, 0xCE, 0xCB, 0xB6, 0x40, 0x68, 0x37, 0xBF, 0x51, 0xF5}}}};