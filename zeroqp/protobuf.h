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
