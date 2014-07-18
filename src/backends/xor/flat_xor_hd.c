/* 
 * <Copyright>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.  THIS SOFTWARE IS PROVIDED BY
 * THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * liberasurecode flat_xor_hd backend
 *
 * vi: set noai tw=79 ts=4 sw=4:
 */

#include <stdio.h>
#include <stdlib.h>
#include <xor_code.h>

#include "erasurecode.h"
#include "erasurecode_backend.h"

/* Forward declarations */
struct ec_backend_op_stubs flat_xor_hd_ops;
struct ec_backend flat_xor_hd;

struct flat_xor_hd_descriptor {
    xor_code_t *xor_desc;

    xor_code_t* (*init_xor_hd_code)(int k, int m, int hd); 
    void (*xor_code_encode)(xor_code_t *code_desc, char **data, char **parity,
            int blocksize); 
    int (*xor_hd_decode)(xor_code_t *code_desc, char **data, char **parity,
            int *missing_idxs, int blocksize, int decode_parity);
    int (*xor_hd_fragments_needed)(xor_code_t *code_desc, int *missing_idxs,
            int *fragments_needed);
};

static int flat_xor_hd_encode(void *desc,
                              char **data, char **parity, int blocksize)
{
    xor_code_t *xor_desc = (xor_code_t *) desc;
    xor_desc->encode(xor_desc, data, parity, blocksize);
}

static int flat_xor_hd_decode(void *desc,
                              char **data, char **parity, int *missing_idxs,
                              int blocksize)
{
    xor_code_t *xor_desc = (xor_code_t *) desc;
    xor_desc->decode(xor_desc, data, parity, missing_idxs, blocksize, 1);
}

static int flat_xor_hd_reconstruct(void *desc,
                                   char **data, char **parity, int *missing_idxs,
                                   int destination_idx, int blocksize)
{
    xor_code_t *xor_desc = (xor_code_t *) desc;
    // (*fptr)(xor_desc, data, parity, missing_idxs, destination_idx, blocksize);
}

static int flat_xor_hd_min_fragments(void *desc,
                                     int *missing_idxs, int *fragments_needed)
{
    xor_code_t *xor_desc = (xor_code_t *) desc;
    xor_desc->fragments_needed(xor_desc, missing_idxs, fragments_needed);
}

#define DEFAULT_W 32
static void * flat_xor_hd_init(struct ec_backend_args *args, void *sohandle)
{
    int k = args->uargs.k;
    int m = args->uargs.m;
    int hd = args->uargs.priv_args1.flat_xor_hd_args.hd;

    /* store w back in args so upper layer can get to it */
    args->uargs.w = DEFAULT_W;

    struct flat_xor_hd_descriptor *bdesc = (struct flat_xor_hd_descriptor *)
        malloc(sizeof(struct flat_xor_hd_descriptor));

    if (NULL == bdesc)
        return NULL;

    xor_code_t *xor_desc = init_xor_hd_code(k, m, hd);
    bdesc->xor_desc = xor_desc;

    return (void *) bdesc;
}

static int flat_xor_hd_exit(void *desc)
{
    struct flat_xor_hd_descriptor *bdesc = (struct flat_xor_hd_descriptor *) desc;

    free (bdesc->xor_desc);
    free (bdesc);
}

struct ec_backend_op_stubs flat_xor_hd_op_stubs = {
    .INIT                       = flat_xor_hd_init,
    .EXIT                       = flat_xor_hd_exit,
    .ENCODE                     = flat_xor_hd_encode,
    .DECODE                     = flat_xor_hd_decode,
    .FRAGSNEEDED                = flat_xor_hd_min_fragments,
    .RECONSTRUCT                = flat_xor_hd_reconstruct,
};

struct ec_backend_common backend_flat_xor_hd = {
    .id                         = EC_BACKEND_FLAT_XOR_HD,
    .name                       = "flat_xor_hd",
    .soname                     = "libXorcode.so",
    .soversion                  = "1.0",
    .ops                        = &flat_xor_hd_op_stubs,
};
