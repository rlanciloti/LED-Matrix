#include <stdio.h>
#include <string.h>
#include "message_headers.h"

uint8_t TX_BUFFER[BUFFER_MAX_SIZE] = { 0 };

int pack_cmd_set_frame(
	uint8_t* msgBuffer, 
	uint32_t msgBufferSize, 
	ColorRGB* colorBuffer, 
	uint16_t colorBufferSize
) {
	if (msgBuffer == NULL) return MSG_BUFFER_NULL;
	if (colorBuffer == NULL) return COLOR_BUFFER_NULL;

	uint32_t colorBufferSizeBytes = sizeof(ColorRGB) * colorBufferSize;
	uint32_t maxSize = sizeof(MessageHeader_T) + sizeof(CommandHeader_T) + colorBufferSizeBytes;
	
	if (maxSize > msgBufferSize) return MSG_BUFFER_TOO_SMALL;
	
	CommandHeader_T cmdHeader = { 
		.command_type = SET_FRAME, 
		.remaining_message_length = colorBufferSizeBytes
	};

	MessageHeader_T msgHeader = {
		.message_type = CMD_LED,
		.remaining_message_length = sizeof(CommandHeader_T) + cmdHeader.remaining_message_length
	};

	uint8_t* msgHeaderStart = msgBuffer;
	uint8_t* cmdHeaderStart = msgHeaderStart + sizeof(MessageHeader_T);
	uint8_t* dataStart = cmdHeaderStart + sizeof(CommandHeader_T);

	memcpy(msgHeaderStart, &msgHeader, sizeof(MessageHeader_T));
	memcpy(cmdHeaderStart, &cmdHeader, sizeof(CommandHeader_T));
	memcpy(dataStart, colorBuffer, colorBufferSizeBytes);

	return maxSize;
}

int unpack_cmd_set_frame(
	uint8_t* msgBuffer,
	uint32_t msgBufferSize,
	ColorRGB* colorBuffer,
	uint16_t colorBufferSize
) {
	if (msgBuffer == NULL) return MSG_BUFFER_NULL;
	if (colorBuffer == NULL) return COLOR_BUFFER_NULL;

	if (msgBufferSize < sizeof(CommandHeader_T) + sizeof(MessageHeader_T)) {
		return MSG_BUFFER_TOO_SMALL;
	}

	uint8_t* msgHeaderStart = msgBuffer;
	uint8_t* cmdHeaderStart = msgHeaderStart + sizeof(MessageHeader_T);
	uint8_t* dataStart = cmdHeaderStart + sizeof(CommandHeader_T);

	MessageHeader_T msgHeader;
	CommandHeader_T cmdHeader;
	uint16_t ledCount;

	memcpy(&msgHeader, msgHeaderStart, sizeof(MessageHeader_T));
	memcpy(&cmdHeader, cmdHeaderStart, sizeof(CommandHeader_T));

	if (msgHeader.message_type != CMD_LED) return MSG_TYPE_NOT_CMD_LED;
	if (cmdHeader.command_type != SET_FRAME) return CMD_TYPE_NOT_SET_FRAME;

	ledCount = (uint16_t) cmdHeader.remaining_message_length / sizeof(ColorRGB);

	if (ledCount > colorBufferSize) return COLOR_BUFFER_TOO_SMALL;
	memcpy(colorBuffer, dataStart, sizeof(ColorRGB) * ledCount);

	return ledCount;
}

int pack_set_animation_frame(
	uint8_t* msgBuffer,
	uint32_t msgBufferSize,
	char* animationId,
	uint32_t animationIdLength
) {
	if (msgBuffer == NULL) return MSG_BUFFER_NULL;
	if (msgBufferSize <
		sizeof(MessageHeader_T) +
		sizeof(CommandHeader_T) +
		sizeof(char) * (animationIdLength + 1)
	) return MSG_BUFFER_TOO_SMALL;

	uint8_t* msgHeaderStart = msgBuffer;
	uint8_t* cmdHeaderStart = msgHeaderStart + sizeof(MessageHeader_T);
	uint8_t* animationIdStart = cmdHeaderStart + sizeof(CommandHeader_T);

	MessageHeader_T msgHeader = {
		.message_type = CMD_LED,
		.remaining_message_length = sizeof(CommandHeader_T) + sizeof(char) * (animationIdLength + 1)
	};

	CommandHeader_T cmdHeader = {
		.command_type = SET_ANIMATION,
		.remaining_message_length = sizeof(char) * (animationIdLength + 1)
	};

	memcpy(msgHeaderStart, &msgHeader, sizeof(MessageHeader_T));
	memcpy(cmdHeaderStart, &cmdHeader, sizeof(CommandHeader_T));
	memcpy(animationIdStart, animationId, sizeof(char) * animationIdLength);

	return sizeof(MessageHeader_T) + sizeof(CommandHeader_T) + sizeof(char) * animationIdLength;
}

int unpack_set_animation_frame(
	uint8_t* msgBuffer,
	uint32_t msgBufferSize,
	char* animationId,
	uint32_t animationIdLength
) {
	if (msgBuffer == NULL) return MSG_BUFFER_NULL;
	if (msgBufferSize <
		sizeof(MessageHeader_T) +
		sizeof(CommandHeader_T) +
		sizeof(char) * animationIdLength
	) return MSG_BUFFER_TOO_SMALL;

	uint8_t* msgHeaderStart = msgBuffer;
	uint8_t* cmdHeaderStart = msgHeaderStart + sizeof(MessageHeader_T);
	uint8_t* animationIdStart = cmdHeaderStart + sizeof(CommandHeader_T);

	MessageHeader_T msgHeader;
	CommandHeader_T cmdHeader;
	uint32_t messageLength;

	memcpy(&msgHeader, msgHeaderStart, sizeof(MessageHeader_T));
	memcpy(&cmdHeader, cmdHeaderStart, sizeof(CommandHeader_T));

	if (msgHeader.message_type != CMD_LED) return MSG_TYPE_NOT_CMD_LED;
	if (cmdHeader.command_type != SET_ANIMATION) return CMD_TYPE_NOT_SET_ANIMATION;
	
	messageLength = cmdHeader.remaining_message_length * sizeof(char);

	if (messageLength > animationIdLength) return BUFFER_LENGTH_TOO_SHORT;

	memcpy(animationId, animationIdStart, messageLength);

	return 0;
}

int pack_animation_ctrl_frame(
	uint8_t* msgBuffer,
	uint32_t msgBufferSize,
	ANIMATION_CTRL_T state
) {
	if (msgBuffer == NULL) return MSG_BUFFER_NULL;
	if (msgBufferSize < 
		sizeof(MessageHeader_T) + sizeof(ANIMATION_CTRL_T)
	) return MSG_BUFFER_TOO_SMALL;

	uint8_t* msgHeaderStart = msgBuffer;
	uint8_t* animationStateStart = msgBuffer + sizeof(MessageHeader_T);

	MessageHeader_T msgHeader = {
		.message_type = ANIMATION_CTRL,
		.remaining_message_length = sizeof(ANIMATION_CTRL_T)
	};

	memcpy(msgHeaderStart, &msgHeader, sizeof(MessageHeader_T));
	memcpy(animationStateStart, &state, sizeof(ANIMATION_CTRL_T));

	return sizeof(MessageHeader_T) + sizeof(ANIMATION_CTRL_T);
}

int unpack_animation_ctrl_frame(
	uint8_t* msgBuffer,
	uint32_t msgBufferSize,
	ANIMATION_CTRL_T* state
) {
	if (msgBuffer == NULL) return MSG_BUFFER_NULL;
	if (msgBufferSize <
		sizeof(MessageHeader_T) + sizeof(ANIMATION_CTRL_T)
	) return MSG_BUFFER_TOO_SMALL;

	uint8_t* msgHeaderStart = msgBuffer;
	uint8_t* animationStateStart = msgBuffer + sizeof(MessageHeader_T);

	MessageHeader_T msgHeader;

	memcpy(&msgHeader, msgHeaderStart, sizeof(MessageHeader_T));

	if (msgHeader.message_type != ANIMATION_CTRL) return MSG_TYPE_NOT_ANIMATION_CTRL;
	if (msgHeader.remaining_message_length < sizeof(ANIMATION_CTRL_T)) return MSG_TOO_SHORT;

	memcpy(state, animationStateStart, sizeof(ANIMATION_CTRL_T));

	return 0;
}

int pack_client_connect_request_handshake_frame(
	uint8_t* msgBuffer,
	uint32_t msgBufferSize
) {
	if (msgBuffer == NULL) return MSG_BUFFER_NULL;
	if (msgBufferSize < sizeof(MessageHeader_T)) return MSG_BUFFER_TOO_SMALL;

	MessageHeader_T msgHeader = {
		.message_type = CONNECT_REQUEST,
		.remaining_message_length = 0
	};

	memcpy(msgBuffer, &msgHeader, sizeof(MessageHeader_T));

	return sizeof(MessageHeader_T);
}

int unpack_client_connect_request_handshake_frame(
	uint8_t* msgBuffer,
	uint32_t msgBufferSize
) {
	if (msgBuffer == NULL) return MSG_BUFFER_NULL;
	if (msgBufferSize < sizeof(MessageHeader_T)) return MSG_BUFFER_TOO_SMALL;

	MessageHeader_T msgHeader;

	memcpy(&msgHeader, msgBuffer, sizeof(MessageHeader_T));

	if (msgHeader.message_type != CONNECT_REQUEST) return MSG_TYPE_NOT_CONNECT_REQUEST;

	return 0;
}

int pack_client_connect_response_handshake_frame(
	uint8_t* msgBuffer,
	uint32_t msgBufferSize,
	uint32_t height,
	uint32_t width,
	char* clientId,
	uint32_t clientIdLength
) {
	if (msgBuffer == NULL) return MSG_BUFFER_NULL;
	if (msgBufferSize < 
		sizeof(MessageHeader_T) + 
		sizeof(uint32_t) * 2 + 
		sizeof(char) * (clientIdLength + 1)
	) return MSG_BUFFER_TOO_SMALL;

	uint8_t* msgHeaderStart = msgBuffer;
	uint8_t* heightStart = msgHeaderStart + sizeof(MessageHeader_T);
	uint8_t* widthStart = heightStart + sizeof(uint32_t);
	uint8_t* clientIdStart = widthStart + sizeof(uint32_t);

	MessageHeader_T msgHeader = {
		.message_type = CONNECT_REPLY,
		.remaining_message_length = clientIdLength + sizeof(uint32_t) * 2 + 1
	};

	memcpy(msgHeaderStart, &msgHeader, sizeof(MessageHeader_T));
	memcpy(heightStart, &height, sizeof(uint32_t));
	memcpy(widthStart, &width, sizeof(uint32_t));
	memcpy(clientIdStart, clientId, sizeof(char) * clientIdLength);

	return sizeof(MessageHeader_T) + sizeof(uint32_t) * 2 + sizeof(char) * clientIdLength;
}

int unpack_client_connect_response_handshake_frame(
	uint8_t* msgBuffer,
	uint32_t msgBufferSize,
	uint32_t* height,
	uint32_t* width,
	char* clientId,
	uint32_t clientIdLength
) {
	if (msgBuffer == NULL) return MSG_BUFFER_NULL;
	if (msgBufferSize <
		sizeof(MessageHeader_T) +
		sizeof(uint32_t) * 2 +
		sizeof(char) * (clientIdLength + 1)
	) return MSG_BUFFER_TOO_SMALL;

	uint8_t* msgHeaderStart = msgBuffer;
	uint8_t* heightStart = msgHeaderStart + sizeof(MessageHeader_T);
	uint8_t* widthStart = heightStart + sizeof(uint32_t);
	uint8_t* clientIdStart = widthStart + sizeof(uint32_t);
	
	uint32_t messageLength;

	MessageHeader_T msgHeader;

	memcpy(&msgHeader, msgHeaderStart, sizeof(MessageHeader_T));

	if (msgHeader.message_type != CONNECT_REPLY) return MSG_TYPE_NOT_CONNECT_REPLY;	
	
	messageLength = msgHeader.remaining_message_length * sizeof(char);
	
	if (messageLength - sizeof(uint32_t) * 2 > clientIdLength) return BUFFER_LENGTH_TOO_SHORT;

	memcpy(height, heightStart, sizeof(uint32_t));
	memcpy(width, widthStart, sizeof(uint32_t));
	memcpy(clientId, clientIdStart, messageLength - sizeof(uint32_t) * 2);
	
	return 0;
}
