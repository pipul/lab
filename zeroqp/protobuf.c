#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "protobuf.h"

ptb_t *protobuf_create()
{
	ptb_t *ptb;

	ptb = malloc(sizeof(ptb_t));
	if (ptb)
		memset(ptb,0,sizeof(ptb_t));
	return(ptb);
}

void protobuf_free(ptb_t *ptb)
{
	if (ptb) {
		while (ptb->ptb_idx) {
			free(ptb->ptb_slice_v[ptb->ptb_idx - 1]);
			ptb->ptb_idx--;
		}
		free(ptb->ptb_slice_v);
	}
	free(ptb);
}

int32_t protobuf_append(ptb_t *ptb, int8_t *s, int32_t n)
{
	if (!ptb || !s || n == 0)
		return(-1);
	if (ptb->ptb_len + n > PROTOBUF_SIZE)
		return(-1);
	memcpy(ptb->ptb_buf + ptb->ptb_len, s, n);
	ptb->ptb_len += n;
	return(0);
}

typedef enum {S0,S1,S2,S3,S4,UNKNOW} ptb_state_t;

int32_t protobuf_parse(ptb_t *ptb)
{
	int32_t pos, slice_len;
	ptb_state_t state = S0;
	int8_t c, sb[PROTOBUF_SIZE] = {0}, *_sb = sb;

	if (!ptb)
		return(PROTOBUF_STATE_ERR);
	pos = ptb->ptb_pos;

	while (!ptb->ptb_slice_c && pos <= ptb->ptb_len) {
		switch (state) {
		case S0:
			c = ptb->ptb_buf[pos++];
			state = (c == '*') ? S1 : UNKNOW;
			break;
		case S1:
			c = ptb->ptb_buf[pos++];
			if (c <= '9' && c >= '0') {
				state = S2;
				*_sb++ = c;
			} else
				state = UNKNOW;
			break;
		case S2:
			c = ptb->ptb_buf[pos++];
			state = (c == '\r') ? S3 : UNKNOW;
			break;
		case S3:
			c = ptb->ptb_buf[pos++];
			state = (c == '\n') ? S4 : UNKNOW;
			break;
		case S4:
			*_sb = '\0';
			ptb->ptb_slice_c = atoi(sb);
			if (ptb->ptb_slice_c < 0 || ptb->ptb_slice_c > 9)
				return(PROTOBUF_STATE_ERR);
			ptb->ptb_pos = pos;
			ptb->ptb_slice_v = calloc(ptb->ptb_slice_c,sizeof(ptb_slice_t **));
			if (!ptb->ptb_slice_v)
				return(PROTOBUF_STATE_ERR);
			/* initial all the env arguments */
			_sb = sb;
			state = S0;
			break;
		case UNKNOW:
			return(PROTOBUF_STATE_ERR);
		}
	}

	while (ptb->ptb_slice_c && pos <= ptb->ptb_len) {
		switch (state) {
		case S0:
			c = ptb->ptb_buf[pos++];
			state = (c == '$') ? S1 : UNKNOW;
			break;
		case S1:
			c = ptb->ptb_buf[pos++];
			if (c <= '9' && c >= '0') {
				state = S2;
				*_sb++ = c;
			} else
				state = UNKNOW;
			break;
		case S2:
			c = ptb->ptb_buf[pos++];
			if (c <= '9' && c >= '0') {
				state = S2;
				*_sb++ = c;
			} else if (c == '\r')
				state = S3;
			else
				state = UNKNOW;
			break;
		case S3:
			c = ptb->ptb_buf[pos++];
			state = (c == '\n') ? S4 : UNKNOW;
			break;
		case S4:
			*_sb = '\0';
			slice_len = atoi(sb);
			if (slice_len < 0 || slice_len > 1024)
				return(PROTOBUF_STATE_ERR);
			if (slice_len + pos + 2 > ptb->ptb_len)
				return(PROTOBUF_STATE_WAIT);
			ptb->ptb_slice_v[ptb->ptb_idx] =
				malloc(slice_len + 1 + sizeof(ptb_slice_t));
			if (!ptb->ptb_slice_v[ptb->ptb_idx])
				return(PROTOBUF_STATE_ERR);
			memset(ptb->ptb_slice_v[ptb->ptb_idx],
				   0,sizeof(ptb_slice_t) + 1 + slice_len);
			ptb->ptb_slice_v[ptb->ptb_idx]->slice_len = slice_len;
			memcpy(ptb->ptb_slice_v[ptb->ptb_idx]->slice_data,
				   ptb->ptb_buf + pos,slice_len);
			ptb->ptb_pos = pos += slice_len + 2;  /* \r\n */
			ptb->ptb_idx++;

			/* initial all the env arguments */
			_sb = sb;
			state = S0;
			break;
		case UNKNOW:
			return(PROTOBUF_STATE_ERR);
		}
		if (ptb->ptb_idx == ptb->ptb_slice_c)
			return(PROTOBUF_STATE_OK);
	}
	return(PROTOBUF_STATE_WAIT);
}

int32_t protobuf_clean(ptb_t *ptb)
{
	int32_t i;

	if (!ptb)
		return(-1);
	while (ptb->ptb_idx) {
		free(ptb->ptb_slice_v[ptb->ptb_idx - 1]);
		ptb->ptb_idx--;
	}
	free(ptb->ptb_slice_v);
	ptb->ptb_slice_v = NULL;
	for (i = 0; i < ptb->ptb_len - ptb->ptb_pos; i++)
		ptb->ptb_buf[i] = ptb->ptb_buf[i + ptb->ptb_pos];
	ptb->ptb_len -= ptb->ptb_pos;
	ptb->ptb_slice_c = ptb->ptb_pos = 0;
	return(0);
}
