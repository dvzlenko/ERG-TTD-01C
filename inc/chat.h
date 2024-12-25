#ifndef __chat_h
#define __chat_h

#include "FreeRTOS.h"
#include "stdio.h"
#include "stdlib.h"
#include "task.h"
#include "semphr.h"
#include "cdcio.h"
#include "string.h"
#include "logger.h"
#include "memory.h"
#include "strtok.h"
#include "version.h"

struct chat_rw_funcs {
	unsigned (*read)(char *s, unsigned len);
	void (*write)(char *s, unsigned len);
};

void vChatTask(void *vpars);

#endif
