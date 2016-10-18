/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*  Fluent Bit
 *  ==========
 *  Copyright (C) 2015-2016 Treasure Data Inc.
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

#include <fluent-bit/flb_info.h>
#include <fluent-bit/flb_utils.h>
#include <fluent-bit/flb_config.h>
#include <fluent-bit/flb_worker.h>
#include <fluent-bit/flb_log.h>

#include <unistd.h>

FLB_TLS_DEFINE(struct flb_worker, flb_worker_ctx);

/*
 * The step_callback runs in a POSIX thread context, it have been started
 * by flb_worker_create(...). Here we setup specific FLB requirements and
 * then we jump into the target/original callback.
 */
static void step_callback(void *data)
{
    int ret;
    struct flb_worker *worker = data;

    /* Set the worker context global */
    FLB_TLS_SET(flb_worker_ctx, worker);

    /* Initialize worker-required data */
    ret = flb_log_worker_init(worker);
    if (ret == -1) {
        fprintf(stderr, "[worker] error initializing log-worker context\n");
    }

    /* not too scary :) */
    worker->func(worker->data);

    /* FIXME: add a good plan for pthread_exit and 'worker' release */
    pthread_exit(NULL);
}

/*
 * Creates a worker (POSIX thread). This function creates a worker
 * context and also setup the 'step' callback to initialize generic
 * Fluent Bit requirements before to invoke the real target callback
 * set by the caller.
 *
 * E.g: We do this intermediary 'step' to initialize the required
 * logging context and possible others.
 */
int flb_worker_create(void (*func) (void *), void *arg, pthread_t *tid,
                      struct flb_config *config)
{
    int ret;
    struct flb_worker *worker;

    worker = malloc(sizeof(struct flb_worker));
    if (!worker) {
        perror("malloc");
        return -1;
    }
    worker->func   = func;
    worker->data   = arg;
    worker->config = config;

    /* Initialize log-specific */
    ret = flb_log_worker_init(worker);
    if (ret == -1) {
        free(worker);
        return -1;
    }

    /* Spawn the step_callback and the func() */
    ret = mk_utils_worker_spawn(step_callback, worker, &worker->tid);
    if (ret != 0) {
        free(worker);
        return -1;
    }
    memcpy(tid, &worker->tid, sizeof(pthread_t));
    mk_list_add(&worker->_head, &config->workers);

    return 0;
}

/*
 * The worker interface aims to prepare any context required by Threads when
 * running, this function is called just one time.
 */
int flb_worker_init(struct flb_config *config)
{
    FLB_TLS_INIT(flb_worker_ctx);

    return 0;
}

/* Lookup a worker using it pthread id */
struct flb_worker *flb_worker_lookup(pthread_t tid, struct flb_config *config)
{
    struct mk_list *head;
    struct flb_worker *worker;

    mk_list_foreach(head, &config->workers) {
        worker = mk_list_entry(head, struct flb_worker, _head);
        if (pthread_equal(worker->tid, tid) != 0) {
            return worker;
        }
    }

    return NULL;
}

struct flb_worker *flb_worker_get()
{
    return FLB_TLS_GET(flb_worker_ctx);
}

int flb_worker_exit(struct flb_config *config)
{
    int c = 0;
    struct mk_list *tmp;
    struct mk_list *head;
    struct flb_worker *worker;

    mk_list_foreach_safe(head, tmp, &config->workers) {
        worker = mk_list_entry(head, struct flb_worker, _head);
        mk_list_del(&worker->_head);
        free(worker);
        c++;
    }

    return c;
}