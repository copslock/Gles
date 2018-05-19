# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#


include $(CLEAR_VARS)

LOCAL_MODULE    := openal


MAX_SOURCES_LOW ?= 4
MAX_SOURCES_START ?= 8
MAX_SOURCES_HIGH ?= 64
#LOCAL_CFLAGS :=-fno-aggressive-loop-optimizations -Werror -DMAX_SOURCES_LOW=$(MAX_SOURCES_LOW) -DMAX_SOURCES_START=$(MAX_SOURCES_START) -DMAX_SOURCES_HIGH=$(MAX_SOURCES_HIGH)
LOCAL_CFLAGS :=-Werror -DMAX_SOURCES_LOW=$(MAX_SOURCES_LOW) -DMAX_SOURCES_START=$(MAX_SOURCES_START) -DMAX_SOURCES_HIGH=$(MAX_SOURCES_HIGH)



LOCAL_CFLAGS    +=  -DAL_ALEXT_PROTOTYPES \
                    -DANDROID \
                    -fpic \
                    -ffunction-sections \
                    -funwind-tables \
                    -fstack-protector \
                    -fno-short-enums \
                    -DHAVE_GCC_VISIBILITY
                    


LOCAL_LDLIBS    := -llog -Wl,--build-id -Bsymbolic -shared

LOCAL_C_INCLUDES :=	src/main/jni/3rdParty/OpenAL-soft-android	\
			src/main/jni/3rdParty/OpenAL-soft-android/include	\
			src/main/jni/3rdParty/OpenAL-soft-android/OpenAL32/Include


LOCAL_SRC_FILES :=  3rdParty/OpenAL-soft-android/Alc/android.c              \
                    3rdParty/OpenAL-soft-android/OpenAL32/alAuxEffectSlot.c \
                    3rdParty/OpenAL-soft-android/OpenAL32/alBuffer.c        \
                    3rdParty/OpenAL-soft-android/OpenAL32/alDatabuffer.c    \
                    3rdParty/OpenAL-soft-android/OpenAL32/alEffect.c        \
                    3rdParty/OpenAL-soft-android/OpenAL32/alError.c         \
                    3rdParty/OpenAL-soft-android/OpenAL32/alExtension.c     \
                    3rdParty/OpenAL-soft-android/OpenAL32/alFilter.c        \
                    3rdParty/OpenAL-soft-android/OpenAL32/alListener.c      \
                    3rdParty/OpenAL-soft-android/OpenAL32/alSource.c        \
                    3rdParty/OpenAL-soft-android/OpenAL32/alState.c         \
                    3rdParty/OpenAL-soft-android/OpenAL32/alThunk.c         \
                    3rdParty/OpenAL-soft-android/Alc/ALc.c                  \
                    3rdParty/OpenAL-soft-android/Alc/alcConfig.c            \
                    3rdParty/OpenAL-soft-android/Alc/alcEcho.c              \
                    3rdParty/OpenAL-soft-android/Alc/alcModulator.c         \
                    3rdParty/OpenAL-soft-android/Alc/alcReverb.c            \
                    3rdParty/OpenAL-soft-android/Alc/alcRing.c              \
                    3rdParty/OpenAL-soft-android/Alc/alcThread.c            \
                    3rdParty/OpenAL-soft-android/Alc/ALu.c                  \
                    3rdParty/OpenAL-soft-android/Alc/bs2b.c                 \
                    3rdParty/OpenAL-soft-android/Alc/null.c                 \
                    3rdParty/OpenAL-soft-android/Alc/panning.c              \
                    3rdParty/OpenAL-soft-android/Alc/mixer.c                \
                    3rdParty/OpenAL-soft-android/Alc/audiotrack.c


include $(BUILD_STATIC_LIBRARY)
