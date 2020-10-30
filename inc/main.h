#if !defined(_MAIN_H)
#define _MAIN_H

#include <app.h>
#include <system_settings.h>
#include <dlog.h>
#include <Elementary.h>
#include <efl_extension.h>

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "tennis-scores"

#if !defined(PACKAGE)
#define PACKAGE "com.matimoro.tennis.scores"
#endif

#endif
