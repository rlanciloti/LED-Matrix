#ifndef MESSAGE_HEADERS_H
#define MESSAGE_HEADERS_H

#include <limits.h>
#include "color.h"

#define BUFFER_MAX_SIZE 8192

typedef enum {
	CMD_LED = 0,
	ANIMATION_CTRL = 1,
	CONNECT_REQUEST = 2, 
	CONNECT_REPLY = 3, 
	LAST_MSG_TYPE
} MESSAGE_TYPE;

typedef enum {
	SET_FRAME = 0,
	SET_ANIMATION = 1,
	LAST_CMD_TYPE
} CMD_TYPE;

typedef enum {
	PAUSE = 0,
	PLAY = 1,
	LAST_ANIMATION_CTRL_T
} ANIMATION_CTRL_T;

typedef struct {
	MESSAGE_TYPE message_type;
	uint32_t remaining_message_length;
} MessageHeader_T;

typedef struct {
	CMD_TYPE command_type;
	uint32_t remaining_message_length;
} CommandHeader_T;

extern uint8_t TX_BUFFER[BUFFER_MAX_SIZE];

enum FRAME_ERRORS {
	MSG_BUFFER_NULL = INT_MIN,
	COLOR_BUFFER_NULL,
	
	MSG_BUFFER_TOO_SMALL,
	COLOR_BUFFER_TOO_SMALL,

	MSG_TYPE_NOT_CMD_LED,
	MSG_TYPE_NOT_CONNECT,
	MSG_TYPE_NOT_ANIMATION_CTRL,
	MSG_TYPE_NOT_CONNECT_REQUEST,
	MSG_TYPE_NOT_CONNECT_REPLY,

	CMD_TYPE_NOT_SET_FRAME,
	CMD_TYPE_NOT_SET_ANIMATION,

	MSG_TOO_SHORT,
	BUFFER_LENGTH_TOO_SHORT
};

int pack_cmd_set_frame(
	uint8_t* msgBuffer,
	uint32_t msgBufferSize,
	ColorRGB* colorBuffer, 
	uint16_t colorBufferSize
);

int unpack_cmd_set_frame(
	uint8_t* msgBuffer,
	uint32_t msgBufferSize,
	ColorRGB* colorBuffer,
	uint16_t colorBufferSize
);

int pack_set_animation_frame(
	uint8_t* msgBuffer,
	uint32_t msgBufferSize,
	char* animationId,
	uint32_t animationIdLength
);

int unpack_set_animation_frame(
	uint8_t* msgBuffer,
	uint32_t msgBufferSize,
	char* animationId,
	uint32_t animationIdLength
);

int pack_animation_ctrl_frame(
	uint8_t* msgBuffer,
	uint32_t msgBufferSize,
	ANIMATION_CTRL_T state
);

int unpack_animation_ctrl_frame(
	uint8_t* msgBuffer,
	uint32_t msgBufferSize,
	ANIMATION_CTRL_T* state
);

int pack_client_connect_request_handshake_frame(
	uint8_t* msgBuffer,
	uint32_t msgBufferSize
);

int unpack_client_connect_request_handshake_frame(
	uint8_t* msgBuffer,
	uint32_t msgBufferSize
);

int pack_client_connect_response_handshake_frame(
	uint8_t* msgBuffer,
	uint32_t msgBufferSize,
	uint32_t height,
	uint32_t width,
	char* clientId,
	uint32_t clientIdLength
);

int unpack_client_connect_response_handshake_frame(
	uint8_t* msgBuffer,
	uint32_t msgBufferSize,
	uint32_t* height,
	uint32_t* width,
	char* clientId,
	uint32_t clientIdLength
);

#endif //MESSAGE_HEADERS_H
