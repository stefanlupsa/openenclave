// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#ifndef _OE_SIGN_H
#define _OE_SIGN_H

#include <openenclave/defs.h>
#include <openenclave/result.h>
#include "sha.h"
#include "sgxtypes.h"

OE_EXTERNC_BEGIN

/**
* Digitially sign the enclave with the given hash
*
* This function digitally signs the enclave whose hash is given by the
* **mrenclave** parameter. The signing key is given by the **pemData**
* parameter. If successful, the function writes the signature into the
* **sigstruct** parameter (an SGX signature structure).
*
* @param mrenclave[in] hash of the enclave being signed
* @param pemData[in] PEM buffer containing the signing key
* @param pemSize[in] size of the PEM buffer
* @param sigstruct[out] the SGX signature 
*
* @return OE_OK success
*/
OE_Result OE_SignEnclave(
    const OE_SHA256* mrenclave,
    const char* pemData,
    size_t pemSize,
    SGX_SigStruct* sigstruct);

OE_EXTERNC_END

#endif /* _OE_SIGN_H */
