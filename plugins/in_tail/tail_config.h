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

#ifndef FLB_TAIL_CONFIG_H
#define FLB_TAIL_CONFIG_H

#include <fluent-bit/flb_info.h>
#include <fluent-bit/flb_input.h>
#include <fluent-bit/flb_parser.h>
#include <fluent-bit/flb_macros.h>

struct flb_tail_config {
    int fd_notify;             /* inotify fd               */
    int ch_manager[2];         /* pipe: channel manager    */
    int ch_pending[2];         /* pipe: pending events     */

    /* Buffer Config */
    size_t buf_chunk_size;     /* allocation chunks        */
    size_t buf_max_size;       /* max size of a buffer     */

    /* Collectors */
    int coll_fd_static;
    int coll_fd_scan;
    int coll_fd_rotated;
    int coll_fd_pending;
    int coll_fd_mult_flush;

    /* Backend collectors */
    int coll_fd_fs1;           /* used by fs_inotify & fs_stat */
    int coll_fd_fs2;           /* only used by fs_stat         */

    /* Configuration */
    int dynamic_tag;           /* dynamic tag ? e.g: abc.*     */
    int refresh_interval_sec;  /* seconds to re-scan           */
    long refresh_interval_nsec;/* nanoseconds to re-scan       */
    int rotate_wait;           /* sec to wait on rotated files */
    int ignore_older;          /* ignore fields older than X seconds        */
    time_t last_pending;       /* last time a 'pending signal' was emitted' */
    char *path;                /* lookup path (glob)           */
    char *exclude_path;        /* exclude path                 */
    char *path_key;            /* key name of file path        */
    int   path_key_len;        /* length of key name           */
    char *key;                 /* key for unstructured record  */
    int   key_len;             /* length of key ^              */
    int   skip_long_lines;     /* skip long lines              */

    /* Database */
    struct flb_sqldb *db;

    /* Parser / Format */
    struct flb_parser *parser;

    /* Multiline */
    int multiline;             /* multiline enabled ?  */
    int multiline_flush;       /* multiline flush/wait */
    struct flb_parser *mult_parser_firstline;
    struct mk_list mult_parsers;

    /* Lists head for files consumed statically (read) and by events (inotify) */
    struct mk_list files_static;
    struct mk_list files_event;

    /* List of rotated files that needs to be removed after 'rotate_wait' */
    struct mk_list files_rotated;

    /* List of shell patterns used to exclude certain file names */
    struct mk_list *exclude_list;

    /* Plugin input instance */
    struct flb_input_instance *i_ins;
};

struct flb_tail_config *flb_tail_config_create(struct flb_input_instance *i_ins,
                                               struct flb_config *config);
int flb_tail_config_destroy(struct flb_tail_config *config);

#endif
