/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*  Fluent Bit
 *  ==========
 *  Copyright (C) 2015-2017 Treasure Data Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef FLB_OUT_LIB
#define FLB_OUT_LIB

enum {
    FLB_OUT_LIB_FMT_MSGPACK = 0,
    FLB_OUT_LIB_FMT_JSON,
    FLB_OUT_LIB_FMT_ERROR,
};

#define FLB_FMT_STR_MSGPACK "msgpack"
#define FLB_FMT_STR_JSON    "json"

struct flb_out_lib_config{
    int format;
    int (*user_callback)(void* data, size_t size);
};

#endif
