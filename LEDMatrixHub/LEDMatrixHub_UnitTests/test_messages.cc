#include <gtest/gtest.h>
#include <string.h>

extern "C" {
	#include "message_headers.h"
}

TEST(MessageTest, TestPackCmdSetFrame) {
	const int LED_COUNT = 16;
	ColorRGB leds[LED_COUNT] = {
		{ 0x00, 0x04, 0x08 }, { 0x10, 0x14, 0x18 }, { 0x20, 0x24, 0x28 }, { 0x30, 0x34, 0x38 },
		{ 0x40, 0x44, 0x48 }, { 0x50, 0x54, 0x58 }, { 0x60, 0x64, 0x68 }, { 0x70, 0x74, 0x78 },
		{ 0x80, 0x84, 0x88 }, { 0x90, 0x94, 0x98 }, { 0xA0, 0xA4, 0xA8 }, { 0xB0, 0xB4, 0xB8 },
		{ 0xC0, 0xC4, 0xC8 }, { 0xD0, 0xD4, 0xD8 }, { 0xE0, 0xE4, 0xE8 }, { 0xF0, 0xF4, 0xF8 },
	};

	uint8_t buffer[2048] = { 0 };
	MessageHeader_T msgHeader;
	CommandHeader_T cmdHeader;
	ColorRGB led;

	uint8_t* msgHeaderStart = buffer;
	uint8_t* cmdHeaderStart = buffer + sizeof(MessageHeader_T);
	uint8_t* dataStart = cmdHeaderStart + sizeof(CommandHeader_T);

	EXPECT_EQ(
		pack_cmd_set_frame(buffer, 2048, leds, LED_COUNT), 
		sizeof(MessageHeader_T) + sizeof(CommandHeader_T) + sizeof(ColorRGB) * LED_COUNT
	);

	memcpy(&msgHeader, msgHeaderStart, sizeof(MessageHeader_T));
	memcpy(&cmdHeader, cmdHeaderStart, sizeof(CommandHeader_T));

	EXPECT_EQ(msgHeader.message_type, CMD_LED);
	EXPECT_EQ(
		msgHeader.remaining_message_length, 
		sizeof(CommandHeader_T) + sizeof(ColorRGB) * LED_COUNT
	);

	EXPECT_EQ(cmdHeader.command_type, SET_FRAME);
	EXPECT_EQ(cmdHeader.remaining_message_length, sizeof(ColorRGB) * LED_COUNT);

	for (int i = 0; i < LED_COUNT; i++) {
		memcpy(&led, dataStart + sizeof(ColorRGB) * i, sizeof(ColorRGB));

		EXPECT_EQ(led.red, leds[i].red);
		EXPECT_EQ(led.green, leds[i].green);
		EXPECT_EQ(led.blue, leds[i].blue);
	}
}

TEST(MessageTest, TestPackCmdSetFrameMsgBufferNull) {
	const int LED_COUNT = 16;
	ColorRGB leds[LED_COUNT] = { 0 };

	uint8_t* buffer = nullptr;

	EXPECT_EQ(pack_cmd_set_frame(buffer, 8, leds, LED_COUNT), MSG_BUFFER_NULL);
}

TEST(MessageTest, TestPackCmdSetFrameColorBufferNull) {
	const int LED_COUNT = 16;
	ColorRGB* leds = nullptr;

	uint8_t buffer[100];

	EXPECT_EQ(pack_cmd_set_frame(buffer, 8, leds, LED_COUNT), COLOR_BUFFER_NULL);
}

TEST(MessageTest, TestPackCmdSetFrameMsgBufferTooSmall) {
	const int LED_COUNT = 16;
	ColorRGB leds[LED_COUNT] = { 0 };

	uint8_t buffer[8] = { 0 };

	EXPECT_EQ(pack_cmd_set_frame(buffer, 8, leds, LED_COUNT), MSG_BUFFER_TOO_SMALL);
}

TEST(MessageTest, TestUnpackCmdSetFrame) {
	const int LED_COUNT = 16;
	ColorRGB leds[LED_COUNT] = {
		{ 0x00, 0x04, 0x08 }, { 0x10, 0x14, 0x18 }, { 0x20, 0x24, 0x28 }, { 0x30, 0x34, 0x38 },
		{ 0x40, 0x44, 0x48 }, { 0x50, 0x54, 0x58 }, { 0x60, 0x64, 0x68 }, { 0x70, 0x74, 0x78 },
		{ 0x80, 0x84, 0x88 }, { 0x90, 0x94, 0x98 }, { 0xA0, 0xA4, 0xA8 }, { 0xB0, 0xB4, 0xB8 },
		{ 0xC0, 0xC4, 0xC8 }, { 0xD0, 0xD4, 0xD8 }, { 0xE0, 0xE4, 0xE8 }, { 0xF0, 0xF4, 0xF8 },
	};

	ColorRGB unpackedLeds[LED_COUNT] = { 0 };

	uint8_t buffer[2048] = { 0 };
	pack_cmd_set_frame(buffer, 2048, leds, LED_COUNT);
	EXPECT_EQ(unpack_cmd_set_frame(buffer, 2048, unpackedLeds, LED_COUNT), LED_COUNT);

	for (int i = 0; i < LED_COUNT; i++) {
		EXPECT_EQ(leds[i].red, unpackedLeds[i].red);
		EXPECT_EQ(leds[i].green, unpackedLeds[i].green);
		EXPECT_EQ(leds[i].blue, unpackedLeds[i].blue);
	}
}

TEST(MessageTest, TestUnpackCmdSetFrameMsgBufferNull) {
	uint8_t* buffer = nullptr;
	ColorRGB leds[8];

	EXPECT_EQ(unpack_cmd_set_frame(buffer, 8, leds, 8), MSG_BUFFER_NULL);
}

TEST(MessageTest, TestUnpackCmdSetFrameColorBufferNull) {
	uint8_t buffer[8];
	ColorRGB* leds = nullptr;

	EXPECT_EQ(unpack_cmd_set_frame(buffer, 8, leds, 8), COLOR_BUFFER_NULL);
}

TEST(MessageTest, TestUnpackCmdSetFrameMsgBufferTooSmall) {
	uint8_t buffer[8];
	ColorRGB leds[8];

	EXPECT_EQ(unpack_cmd_set_frame(buffer, 8, leds, 8), MSG_BUFFER_TOO_SMALL);
}

TEST(MessageTest, TestUnpackCmdSetFrameMsgTypeNotCmdLed) {
	uint32_t bufferSize = sizeof(MessageHeader_T) + sizeof(CommandHeader_T) + sizeof(ColorRGB);
	uint8_t* buffer = (uint8_t*) malloc(bufferSize);

	EXPECT_NE(buffer, nullptr);

	MessageHeader_T msgHeader = {
		.message_type = LAST_MSG_TYPE,
		.remaining_message_length = sizeof(CommandHeader_T) + sizeof(ColorRGB)
	};

	CommandHeader_T cmdHeader {
		.command_type = SET_FRAME,
		.remaining_message_length = sizeof(ColorRGB)
	};

	uint8_t* msgHeaderStart = buffer;
	uint8_t* cmdHeaderStart = msgHeaderStart + sizeof(MessageHeader_T);
	uint8_t* dataStart = cmdHeaderStart + sizeof(CommandHeader_T);

	ColorRGB color = { .blue = 255, .red = 100, .green = 87 };
	ColorRGB leds[1] = { 0 };

	if (
		msgHeaderStart != nullptr &&
		cmdHeaderStart != nullptr &&
		dataStart != nullptr
	) {
		memcpy(msgHeaderStart, &msgHeader, sizeof(MessageHeader_T));
		memcpy(cmdHeaderStart, &cmdHeader, sizeof(CommandHeader_T));
		memcpy(dataStart, &color, sizeof(ColorRGB));
	}

	EXPECT_EQ(unpack_cmd_set_frame(buffer, bufferSize, leds, 1), MSG_TYPE_NOT_CMD_LED);

	free(buffer);
}

TEST(MessageTest, TestUnpackCmdSetFrameCmdTypeNotSetFrame) {
	uint32_t bufferSize = sizeof(MessageHeader_T) + sizeof(CommandHeader_T) + sizeof(ColorRGB);
	uint8_t* buffer = (uint8_t*) malloc(bufferSize);

	EXPECT_NE(buffer, nullptr);

	MessageHeader_T msgHeader = {
		.message_type = CMD_LED,
		.remaining_message_length = sizeof(CommandHeader_T) + sizeof(ColorRGB)
	};

	CommandHeader_T cmdHeader {
		.command_type = LAST_CMD_TYPE,
		.remaining_message_length = sizeof(ColorRGB)
	};

	uint8_t* msgHeaderStart = buffer;
	uint8_t* cmdHeaderStart = msgHeaderStart + sizeof(MessageHeader_T);
	uint8_t* dataStart = cmdHeaderStart + sizeof(CommandHeader_T);

	ColorRGB color = { .blue = 255, .red = 100, .green = 87 };
	ColorRGB leds[1] = { 0 };

	if (
		msgHeaderStart != nullptr &&
		cmdHeaderStart != nullptr &&
		dataStart != nullptr
	) {
		memcpy(msgHeaderStart, &msgHeader, sizeof(MessageHeader_T));
		memcpy(cmdHeaderStart, &cmdHeader, sizeof(CommandHeader_T));
		memcpy(dataStart, &color, sizeof(ColorRGB));
	}

	EXPECT_EQ(unpack_cmd_set_frame(buffer, bufferSize, leds, 1), CMD_TYPE_NOT_SET_FRAME);

	free(buffer);
}

TEST(MessageTest, TestUnpackCmdSetFrameColorBufferTooSmall) {
	uint32_t bufferSize = sizeof(MessageHeader_T) + sizeof(CommandHeader_T) + sizeof(ColorRGB) * 2;
	uint8_t* buffer = (uint8_t*) malloc(bufferSize);

	EXPECT_NE(buffer, nullptr);

	MessageHeader_T msgHeader = {
		.message_type = CMD_LED,
		.remaining_message_length = sizeof(CommandHeader_T) + sizeof(ColorRGB) * 2
	};

	CommandHeader_T cmdHeader {
		.command_type = SET_FRAME,
		.remaining_message_length = sizeof(ColorRGB) * 2
	};

	uint8_t* msgHeaderStart = buffer;
	uint8_t* cmdHeaderStart = msgHeaderStart + sizeof(MessageHeader_T);
	uint8_t* dataStart = cmdHeaderStart + sizeof(CommandHeader_T);

	ColorRGB colors[2] = {
		{ .blue = 255, .red = 100, .green = 87 }, 
		{ .blue = 157, .red = 32, .green = 86 } 
	};

	ColorRGB leds[1] = { 0 };

	if (
		msgHeaderStart != nullptr &&
		cmdHeaderStart != nullptr &&
		dataStart != nullptr
		) {
		memcpy(msgHeaderStart, &msgHeader, sizeof(MessageHeader_T));
		memcpy(cmdHeaderStart, &cmdHeader, sizeof(CommandHeader_T));
		memcpy(dataStart, colors, sizeof(ColorRGB) * 2);
	}

	EXPECT_EQ(unpack_cmd_set_frame(buffer, bufferSize, leds, 1), COLOR_BUFFER_TOO_SMALL);

	free(buffer);
}

TEST(MessageTest, TestPackSetAnimationFrame) {
	uint8_t bufferSize =
		sizeof(MessageHeader_T) +
		sizeof(CommandHeader_T) +
		sizeof(char) * 20;
	
	uint8_t* buffer = (uint8_t*) calloc(bufferSize, sizeof(uint8_t));
	char animation[] = "rainbow";
	int animationLength = strlen(animation);

	EXPECT_EQ(
		pack_set_animation_frame(buffer, bufferSize, animation, animationLength), 
		sizeof(MessageHeader_T) + sizeof(CommandHeader_T) + animationLength
	);
	
	uint8_t expected[] = {
		CMD_LED, 0, 0, 0, sizeof(CommandHeader_T) + animationLength + 1, 0, 0, 0,
		SET_ANIMATION, 0, 0, 0, animationLength + 1, 0, 0, 0,
		'r', 'a', 'i', 'n', 'b', 'o', 'w', 0
	};

	for (int i = 0; i < sizeof(expected); i++) EXPECT_EQ(buffer[i], expected[i]);

	free(buffer);
}

TEST(MessageTest, TestUnpackSetAnimationFrame) {
	uint8_t bufferSize =
		sizeof(MessageHeader_T) +
		sizeof(CommandHeader_T) +
		sizeof(char) * 20;

	uint8_t* buffer = (uint8_t*) calloc(bufferSize, sizeof(uint8_t));
	char animation[] = "rainbow";
	int animationLength = strlen(animation);

	EXPECT_EQ(
		pack_set_animation_frame(buffer, bufferSize, animation, animationLength), 
		sizeof(MessageHeader_T) + sizeof(CommandHeader_T) + sizeof(char) * animationLength
	);

	char* animationId = (char*) calloc(20, sizeof(char));

	EXPECT_EQ(unpack_set_animation_frame(buffer, bufferSize, animationId, 20), 0);

	EXPECT_STREQ(animationId, "rainbow");
}

TEST(MessageTest, TestPackAnimationCtrlFrame) {
	uint8_t bufferSize = sizeof(MessageHeader_T) + sizeof(ANIMATION_CTRL_T);
	uint8_t* buffer = (uint8_t*) calloc(bufferSize, sizeof(uint8_t));

	EXPECT_EQ(pack_animation_ctrl_frame(buffer, bufferSize, PLAY), bufferSize);

	uint8_t expected[] = {
		ANIMATION_CTRL, 0, 0, 0, sizeof(ANIMATION_CTRL_T), 0, 0, 0,
		PLAY, 0, 0, 0
	};

	for (int i = 0; i < sizeof(expected); i++) EXPECT_EQ(buffer[i], expected[i]);
}

TEST(MessageTest, TestUnpackAnimationCtrlFrame) {
	uint8_t bufferSize = sizeof(MessageHeader_T) + sizeof(ANIMATION_CTRL_T);
	uint8_t* buffer = (uint8_t*) calloc(bufferSize, sizeof(uint8_t));

	EXPECT_EQ(pack_animation_ctrl_frame(buffer, bufferSize, PLAY), bufferSize);

	ANIMATION_CTRL_T state;

	EXPECT_EQ(unpack_animation_ctrl_frame(buffer, bufferSize, &state), 0);
	EXPECT_EQ(state, PLAY);
}

TEST(MessageTest, TestPackClientConnectRequestHandshakeFrame) {
	uint8_t bufferSize = sizeof(MessageHeader_T);
	uint8_t* buffer = (uint8_t*) calloc(bufferSize, sizeof(uint8_t));

	EXPECT_EQ(pack_client_connect_request_handshake_frame(buffer, bufferSize), bufferSize);

	uint8_t expected[] = { CONNECT_REQUEST, 0, 0, 0, 0, 0, 0, 0 };

	for (int i = 0; i < sizeof(expected); i++) EXPECT_EQ(buffer[i], expected[i]);
}

TEST(MessageTest, TestUnpackClientConnectRequestHandshakeFrame) {
	uint8_t bufferSize = sizeof(MessageHeader_T);
	uint8_t* buffer = (uint8_t*) calloc(bufferSize, sizeof(uint8_t));

	EXPECT_EQ(pack_client_connect_request_handshake_frame(buffer, bufferSize), bufferSize);
	EXPECT_EQ(unpack_client_connect_request_handshake_frame(buffer, bufferSize), 0);
}

TEST(MessageTest, TestPackClientConnectReplyHandshakeFrame) {
	uint8_t bufferSize = sizeof(MessageHeader_T) + sizeof(char) * 20;
	uint8_t* buffer = (uint8_t*) calloc(bufferSize, sizeof(uint8_t));

	char clientId[] = "ESP32";
	uint32_t clientIdLength = strlen(clientId);

	EXPECT_EQ(pack_client_connect_response_handshake_frame(
		buffer, bufferSize, 88, 72, clientId, clientIdLength
	), sizeof(MessageHeader_T) + sizeof(uint32_t) * 2 + sizeof(char) * clientIdLength);

	uint8_t expected[] = { 
		CONNECT_REPLY, 0, 0, 0, clientIdLength + 1 + sizeof(uint32_t) * 2, 0, 0, 0,
		88, 0, 0, 0, 72, 0, 0, 0,
		'E', 'S', 'P', '3', '2', 0
	};

	for (int i = 0; i < sizeof(expected); i++) EXPECT_EQ(buffer[i], expected[i]);
}

TEST(MessageTest, TestUnpackClientConnectReplyHandshakeFrame) {
	uint8_t bufferSize = sizeof(MessageHeader_T) + sizeof(uint32_t) * 2 + sizeof(char) * 20;
	uint8_t* buffer = (uint8_t*) calloc(bufferSize, sizeof(uint8_t));

	char clientId[] = "ESP32";
	uint32_t clientIdLength = strlen(clientId);

	EXPECT_EQ(pack_client_connect_response_handshake_frame(
		buffer, bufferSize, 47, 38, clientId, clientIdLength
	), sizeof(MessageHeader_T) + sizeof(uint32_t) * 2 + sizeof(char) * clientIdLength);

	char parsedClientId[10] = { 0 };
	uint32_t height;
	uint32_t width;

	EXPECT_EQ(unpack_client_connect_response_handshake_frame(
		buffer, bufferSize, &height, &width, parsedClientId, 10
	), 0);

	EXPECT_EQ(height, 47);
	EXPECT_EQ(width, 38);
	EXPECT_STREQ(parsedClientId, "ESP32");
}
