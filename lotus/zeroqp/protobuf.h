/* protobuf.h - A redis-like protocol implementation
 *
 * Copyright (c) 2011-2012, FangDong <yp.fangdong@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef __PROTOBUF_H_
#define __PROTOBUF_H_


#define PROTOBUF_SIZE 10240

typedef struct {
	int32_t slice_len;
	int8_t slice_data[];
} ptb_slice_t;

typedef struct {
	int32_t ptb_slice_c;
	ptb_slice_t **ptb_slice_v;
	int8_t ptb_buf[PROTOBUF_SIZE];
	int32_t ptb_len,ptb_pos,ptb_idx;
} ptb_t;


ptb_t *protobuf_create();
void protobuf_free(ptb_t *ptb);

#define PROTOBUF_STATE_OK 0
#define PROTOBUF_STATE_ERR -1
#define PROTOBUF_STATE_WAIT 1
int32_t protobuf_parse(ptb_t *ptb);

int32_t protobuf_clean(ptb_t *ptb);
int32_t protobuf_append(ptb_t *ptb, int8_t *s, int32_t n);






#endif
