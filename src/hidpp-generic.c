/*
 * HID++ generic definitions
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/*
 * Based on the HID++ documentation provided by Nestor Lopez Casado at:
 *   https://drive.google.com/folderview?id=0BxbRzx7vEV7eWmgwazJ3NUFfQ28&usp=sharing
 */

#include "config.h"

#include "hidpp-generic.h"

#include <errno.h>
#include <poll.h>
#include <stddef.h>

#include "libratbag-private.h"

const char *hidpp10_errors[0x100] = {
	[0x00] = "ERR_SUCCESS",
	[0x01] = "ERR_INVALID_SUBID",
	[0x02] = "ERR_INVALID_ADDRESS",
	[0x03] = "ERR_INVALID_VALUE",
	[0x04] = "ERR_CONNECT_FAIL",
	[0x05] = "ERR_TOO_MANY_DEVICES",
	[0x06] = "ERR_ALREADY_EXISTS",
	[0x07] = "ERR_BUSY",
	[0x08] = "ERR_UNKNOWN_DEVICE",
	[0x09] = "ERR_RESOURCE_ERROR",
	[0x0A] = "ERR_REQUEST_UNAVAILABLE",
	[0x0B] = "ERR_INVALID_PARAM_VALUE",
	[0x0C] = "ERR_WRONG_PIN_CODE",
	[0x0D ... 0xFF] = NULL,
};

const char *hidpp20_errors[0x100] = {
	[0x00] = "ERR_NO_ERROR",
	[0x01] = "ERR_UNKNOWN",
	[0x02] = "ERR_INVALID_ARGUMENT",
	[0x03] = "ERR_OUT_OF_RANGE",
	[0x04] = "ERR_HARDWARE_ERROR",
	[0x05] = "ERR_LOGITECH_INTERNAL",
	[0x06] = "ERR_INVALID_FEATURE_INDEX",
	[0x07] = "ERR_INVALID_FUNCTION_ID",
	[0x08] = "ERR_BUSY",
	[0x09] = "ERR_UNSUPPORTED",
	[0x0A ... 0xFF] = NULL,
};

struct hidpp20_1b04_action_mapping {
	uint16_t value;
	const char *name;
	struct ratbag_button_action action;
};

#define BUTTON_ACTION_MAPPING(v_, n_, act_) \
 { v_, n_, act_ }

static const struct hidpp20_1b04_action_mapping hidpp20_1b04_logical_mapping[] =
{
	BUTTON_ACTION_MAPPING(0, "None"			, BUTTON_ACTION_NONE),
	BUTTON_ACTION_MAPPING(1, "Volume Up"		, BUTTON_ACTION_KEY(KEY_VOLUMEUP)),
	BUTTON_ACTION_MAPPING(2, "Volume Down"		, BUTTON_ACTION_KEY(KEY_VOLUMEDOWN)),
	BUTTON_ACTION_MAPPING(3, "Mute"			, BUTTON_ACTION_KEY(KEY_MUTE)),
	BUTTON_ACTION_MAPPING(4, "Play/Pause"		, BUTTON_ACTION_KEY(KEY_PLAYPAUSE)),
	BUTTON_ACTION_MAPPING(5, "Next"			, BUTTON_ACTION_KEY(KEY_NEXTSONG)),
	BUTTON_ACTION_MAPPING(6, "Previous"		, BUTTON_ACTION_KEY(KEY_PREVIOUSSONG)),
	BUTTON_ACTION_MAPPING(7, "Stop"			, BUTTON_ACTION_KEY(KEY_STOPCD)),
	BUTTON_ACTION_MAPPING(80, "Left"		, BUTTON_ACTION_BUTTON(1)),
	BUTTON_ACTION_MAPPING(81, "Right"		, BUTTON_ACTION_BUTTON(2)),
	BUTTON_ACTION_MAPPING(82, "Middle"		, BUTTON_ACTION_BUTTON(3)),
	BUTTON_ACTION_MAPPING(83, "Back"		, BUTTON_ACTION_BUTTON(4)),
	BUTTON_ACTION_MAPPING(86, "Forward"		, BUTTON_ACTION_BUTTON(5)),
	BUTTON_ACTION_MAPPING(89, "Button 6"		, BUTTON_ACTION_BUTTON(6)),
	BUTTON_ACTION_MAPPING(90, "Button 7"		, BUTTON_ACTION_BUTTON(7)),
	BUTTON_ACTION_MAPPING(91, "Left Scroll"		, BUTTON_ACTION_SPECIAL(RATBAG_BUTTON_ACTION_SPECIAL_WHEEL_LEFT)),
	BUTTON_ACTION_MAPPING(92, "Button 8"		, BUTTON_ACTION_BUTTON(8)),
	BUTTON_ACTION_MAPPING(93, "Right Scroll"	, BUTTON_ACTION_SPECIAL(RATBAG_BUTTON_ACTION_SPECIAL_WHEEL_RIGHT)),
	BUTTON_ACTION_MAPPING(94, "Button 9"		, BUTTON_ACTION_BUTTON(9)),
	BUTTON_ACTION_MAPPING(95, "Button 10"		, BUTTON_ACTION_BUTTON(10)),
	BUTTON_ACTION_MAPPING(96, "Button 11"		, BUTTON_ACTION_BUTTON(11)),
	BUTTON_ACTION_MAPPING(97, "Button 12"		, BUTTON_ACTION_BUTTON(12)),
	BUTTON_ACTION_MAPPING(98, "Button 13"		, BUTTON_ACTION_BUTTON(13)),
	BUTTON_ACTION_MAPPING(99, "Button 14"		, BUTTON_ACTION_BUTTON(14)),
	BUTTON_ACTION_MAPPING(100, "Button 15"		, BUTTON_ACTION_BUTTON(15)),
	BUTTON_ACTION_MAPPING(101, "Button 16"		, BUTTON_ACTION_BUTTON(16)),
	BUTTON_ACTION_MAPPING(102, "Button 17"		, BUTTON_ACTION_BUTTON(17)),
	BUTTON_ACTION_MAPPING(103, "Button 18"		, BUTTON_ACTION_BUTTON(18)),
	BUTTON_ACTION_MAPPING(104, "Button 19"		, BUTTON_ACTION_BUTTON(19)),
	BUTTON_ACTION_MAPPING(105, "Button 20"		, BUTTON_ACTION_BUTTON(20)),
	BUTTON_ACTION_MAPPING(106, "Button 21"		, BUTTON_ACTION_BUTTON(21)),
	BUTTON_ACTION_MAPPING(107, "Button 22"		, BUTTON_ACTION_BUTTON(22)),
	BUTTON_ACTION_MAPPING(108, "Button 23"		, BUTTON_ACTION_BUTTON(23)),
	BUTTON_ACTION_MAPPING(109, "Button 24"		, BUTTON_ACTION_BUTTON(24)),
	BUTTON_ACTION_MAPPING(184, "Second Left"	, BUTTON_ACTION_BUTTON(1)),
	BUTTON_ACTION_MAPPING(195, "AppSwitchGesture"	, BUTTON_ACTION_NONE),
	BUTTON_ACTION_MAPPING(196, "SmartShift"		, BUTTON_ACTION_SPECIAL(RATBAG_BUTTON_ACTION_SPECIAL_RATCHET_MODE_SWITCH)),
	BUTTON_ACTION_MAPPING(315, "LedToggle"		, BUTTON_ACTION_NONE),
};

struct hidpp20_1b04_physical_mapping {
	uint16_t value;
	const char *name;
	enum ratbag_button_type type;
};

#define BUTTON_PHYS_MAPPING(v_, n_, t_) \
 { v_, n_, t_ }

static const struct hidpp20_1b04_physical_mapping hidpp20_1b04_physical_mapping[] =
{
	BUTTON_PHYS_MAPPING(0, "None"			, RATBAG_BUTTON_TYPE_UNKNOWN),
	BUTTON_PHYS_MAPPING(1, "Volume Up"		, RATBAG_BUTTON_TYPE_UNKNOWN),
	BUTTON_PHYS_MAPPING(2, "Volume Down"		, RATBAG_BUTTON_TYPE_UNKNOWN),
	BUTTON_PHYS_MAPPING(3, "Mute"			, RATBAG_BUTTON_TYPE_UNKNOWN),
	BUTTON_PHYS_MAPPING(4, "Play/Pause"		, RATBAG_BUTTON_TYPE_UNKNOWN),
	BUTTON_PHYS_MAPPING(5, "Next"			, RATBAG_BUTTON_TYPE_UNKNOWN),
	BUTTON_PHYS_MAPPING(6, "Previous"		, RATBAG_BUTTON_TYPE_UNKNOWN),
	BUTTON_PHYS_MAPPING(7, "Stop"			, RATBAG_BUTTON_TYPE_UNKNOWN),
	BUTTON_PHYS_MAPPING(56, "Left Click"		, RATBAG_BUTTON_TYPE_LEFT),
	BUTTON_PHYS_MAPPING(57, "Right Click"		, RATBAG_BUTTON_TYPE_RIGHT),
	BUTTON_PHYS_MAPPING(58, "Middle Click"		, RATBAG_BUTTON_TYPE_MIDDLE),
	BUTTON_PHYS_MAPPING(59, "Wheel Side Click Left"	, RATBAG_BUTTON_TYPE_UNKNOWN),
	BUTTON_PHYS_MAPPING(60, "Back Click"		, RATBAG_BUTTON_TYPE_SIDE),
	BUTTON_PHYS_MAPPING(61, "Wheel Side Click Right", RATBAG_BUTTON_TYPE_UNKNOWN),
	BUTTON_PHYS_MAPPING(62, "Forward Click"		, RATBAG_BUTTON_TYPE_EXTRA),
	BUTTON_PHYS_MAPPING(63, "Left Scroll"		, RATBAG_BUTTON_TYPE_WHEEL_LEFT),
	BUTTON_PHYS_MAPPING(64, "Right Scroll"		, RATBAG_BUTTON_TYPE_WHEEL_RIGHT),
	BUTTON_PHYS_MAPPING(98, "Do Nothing"		, RATBAG_BUTTON_TYPE_UNKNOWN),
	BUTTON_PHYS_MAPPING(156, "Gesture Button"	, RATBAG_BUTTON_TYPE_UNKNOWN),
	BUTTON_PHYS_MAPPING(157, "SmartShift"		, RATBAG_BUTTON_TYPE_WHEEL_RATCHET_MODE_SHIFT),
	BUTTON_PHYS_MAPPING(169, "Gesture Button"	, RATBAG_BUTTON_TYPE_MIDDLE),
	BUTTON_PHYS_MAPPING(221, "LedToggle"		, RATBAG_BUTTON_TYPE_UNKNOWN),
};

struct hidpp20_8070_location_mapping {
	uint16_t value;
	const char *name;
	enum ratbag_led_type type;
};

#define LED_LOC_MAPPING(v_, n_, t_) \
 { v_, n_, t_ }

static const struct hidpp20_8070_location_mapping hidpp20_8070_location_mapping[] = {
	LED_LOC_MAPPING(0, "None", RATBAG_LED_TYPE_UNKNOWN),
	LED_LOC_MAPPING(1, "Logo LED", RATBAG_LED_TYPE_LOGO),
	LED_LOC_MAPPING(2, "Side LED", RATBAG_LED_TYPE_SIDE),
};

const struct ratbag_button_action *
hidpp20_1b04_get_logical_mapping(uint16_t value)
{
	const struct hidpp20_1b04_action_mapping *map;

	ARRAY_FOR_EACH(hidpp20_1b04_logical_mapping, map) {
		if (map->value == value)
			return &map->action;
	}

	return NULL;
}

uint16_t
hidpp20_1b04_get_logical_control_id(const struct ratbag_button_action *action)
{
	const struct hidpp20_1b04_action_mapping *mapping;

	ARRAY_FOR_EACH(hidpp20_1b04_logical_mapping, mapping) {
		if (ratbag_button_action_match(&mapping->action, action))
			return mapping->value;
	}

	return 0;
}

const char *
hidpp20_1b04_get_logical_mapping_name(uint16_t value)
{
	const struct hidpp20_1b04_action_mapping *mapping;

	ARRAY_FOR_EACH(hidpp20_1b04_logical_mapping, mapping) {
		if (mapping->value == value)
			return mapping->name;
	}

	return "UNKNOWN";
}

enum ratbag_button_type
hidpp20_1b04_get_physical_mapping(uint16_t value)
{
	const struct hidpp20_1b04_physical_mapping *map;

	ARRAY_FOR_EACH(hidpp20_1b04_physical_mapping, map) {
		if (map->value == value)
			return map->type;
	}

	return RATBAG_BUTTON_TYPE_UNKNOWN;
}

const char *
hidpp20_1b04_get_physical_mapping_name(uint16_t value)
{
	const struct hidpp20_1b04_physical_mapping *map;

	ARRAY_FOR_EACH(hidpp20_1b04_physical_mapping, map) {
		if (map->value == value)
			return map->name;
	}

	return "UNKNOWN";
}

enum ratbag_led_type
hidpp20_led_get_location_mapping(uint16_t value)
{
	const struct hidpp20_8070_location_mapping *map;

	ARRAY_FOR_EACH(hidpp20_8070_location_mapping, map) {
		if (map->value == value)
			return map->type;
	}

	return RATBAG_LED_TYPE_UNKNOWN;
}

const char *
hidpp20_led_get_location_mapping_name(uint16_t value)
{
	const struct hidpp20_8070_location_mapping *map;

	ARRAY_FOR_EACH(hidpp20_8070_location_mapping, map) {
		if (map->value == value)
			return map->name;
	}

	return "UNKNOWN";
}

int
hidpp_write_command(struct hidpp_device *dev, uint8_t *cmd, int size)
{
	int fd = dev->hidraw_fd;
	int res;

	if (size < 1 || !cmd || fd < 0)
		return -EINVAL;

	hidpp_log_buf_raw(dev, "hidpp write: ", cmd, size);
	res = write(fd, cmd, size);
	if (res < 0) {
		res = -errno;
		hidpp_log_error(dev, "Error: %s (%d)\n", strerror(-res), -res);
	}

	return res < 0 ? res : 0;
}

int
hidpp_read_response(struct hidpp_device *dev, uint8_t *buf, size_t size)
{
	int fd = dev->hidraw_fd;
	struct pollfd fds;
	int rc;

	if (size < 1 || !buf || fd < 0)
		return -EINVAL;

	fds.fd = fd;
	fds.events = POLLIN;

	rc = poll(&fds, 1, 1000);
	if (rc == -1)
		return -errno;

	if (rc == 0)
		return -ETIMEDOUT;

	rc = read(fd, buf, size);

	if (rc > 0)
		hidpp_log_buf_raw(dev, "hidpp read:  ", buf, rc);

	return rc >= 0 ? rc : -errno;
}

/*
int
hidpp_test_report(struct hidpp_device *dev, uint8_t device_index, uint8_t report_type, uint report_size)
{
	if (report_size < 4)
		return -EINVAL;

	if (report_size > LONG_MESSAGE_LENGTH) /* this should be the longest, at least for now /
	{
		hidpp_log_error(dev, "We are testing a report type with size bigger than what is supported! Please report this to the libratbag developers.\n");
		return -EINVAL;
	}

	hidpp_log_debug(dev, "");

	int ret;
	_cleanup_(freep) uint8_t *buf = zalloc(LONG_MESSAGE_LENGTH);

	/* we are sending the HID++ version identification routine, please refeer to the docs for more information /
	buf[0] = report_type;
	buf[1] = device_index;
	buf[2] = 0x00; /* RegisterAccessID / IRoot /
	buf[3] = 0x10; /* ? / GetProtocolVersion() /

	ret = hidpp_write_command(dev, buf, report_size);
	if (ret)
		return 0;

	ret = hidpp_read_response(dev, buf, LONG_MESSAGE_LENGTH);

	/* wait and retry if the USB timed out /
	if (ret == -ETIMEDOUT) {
		msleep(10);
		ret = hidpp_read_response(dev, buf, LONG_MESSAGE_LENGTH);
	}

	/* if it didn't reply by now we assume the device does not support this report type /
	if (ret == -ETIMEDOUT)
		return 0;

	return 1;
}
*/

int
hidpp_test_report(struct hidpp_device *dev, uint8_t report_type)
{
	if (!dev->hid_reports)
	{
		hidpp_log_error(dev, "hidpp: we don't have information about the hid reports, marking 0x%02x as unsupported\n", report_type);
		return 0;
	}

	for (unsigned i = 0; i < dev->num_hid_reports; i++)
		hidpp_log_debug(dev, "hid report: id = 0x%02x, usage_page = 0x%02x, usage = 0x%02x\n", dev->hid_reports[i].report_id,
												       dev->hid_reports[i].usage_page,
												       dev->hid_reports[i].usage);
	return 0;
}

void
hidpp_get_supported_report_types(struct hidpp_device *dev, uint8_t device_index)
{
	dev->supported_report_types &= (0xff << 2); /* reset the bits we are gonna check */

	if (hidpp_test_report(dev, REPORT_ID_SHORT))
		dev->supported_report_types |= HIDPP_REPORT_SHORT;

	if (hidpp_test_report(dev, REPORT_ID_LONG))
		dev->supported_report_types |= HIDPP_REPORT_LONG;

	if (dev->supported_report_types & HIDPP_REPORT_SHORT)
		hidpp_log_debug(dev, "hidpp: device supports short reports.\n");
	if (dev->supported_report_types & HIDPP_REPORT_LONG)
		hidpp_log_debug(dev, "hidpp: device supports long reports.\n");
}

void
hidpp_log(struct hidpp_device *dev,
	  enum hidpp_log_priority priority,
	  const char *format,
	  ...)
{
	va_list args;

	if (dev->log_priority > priority)
		return;

	va_start(args, format);
	dev->log_handler(dev->userdata, priority, format, args);
	va_end(args);
}

char *
hidpp_buffer_to_string(const uint8_t *buf, size_t len)
{
	size_t dstlen = len * 6;
	char *dst = zalloc(dstlen);
	unsigned int n = 0;
	char *sep = "";

	for (unsigned i = 0; i < len; ++i) {
		n += snprintf_safe(&dst[n], dstlen - n, "%s%02x", sep, buf[i] & 0xFF);
		sep = " ";
	}
	return dst;
}

void
hidpp_log_buffer(struct hidpp_device *dev,
		 enum hidpp_log_priority priority,
		 const char *header,
		 uint8_t *buf, size_t len)
{
	_cleanup_free_ char *output_buf = NULL;
	_cleanup_free_ char *bytes = NULL;

	bytes = hidpp_buffer_to_string(buf, len);
	asprintf(&output_buf, "%s %s", header ? header : "", bytes);

	hidpp_log(dev, priority, "%s\n", output_buf);
}

static void
simple_log(void *userdata, enum hidpp_log_priority priority, const char *format, va_list args)
{
	vprintf(format, args);
}

void
hidpp_device_init(struct hidpp_device *dev, int fd)
{
	dev->hidraw_fd = fd;
	dev->hid_reports = NULL;
	dev->num_hid_reports = 0;
	hidpp_device_set_log_handler(dev, simple_log, HIDPP_LOG_PRIORITY_INFO, NULL);
	dev->supported_report_types = 0;
}

void
hidpp_device_set_log_handler(struct hidpp_device *dev,
			     hidpp_log_handler log_handler,
			     enum hidpp_log_priority priority,
			     void *userdata)
{
	dev->log_handler = log_handler;
	dev->log_priority = priority;
	dev->userdata = userdata;
}

/*
 * The following crc computation has been provided by Logitech
 */
#define CRC_CCITT_SEED	0xFFFF

uint16_t
hidpp_crc_ccitt(uint8_t *data, unsigned int length)
{
	uint16_t crc, temp, quick;
	unsigned int i;

	crc = CRC_CCITT_SEED;

	for (i = 0; i < length; i++) {
		temp = (crc >> 8) ^ (*data++);
		crc <<= 8;
		quick = temp ^ (temp >> 4);
		crc ^= quick;
		quick <<= 5;
		crc ^= quick;
		quick <<= 7;
		crc ^= quick;
	}

	return crc;
}
