/* ====================================================================
 * The Apache Software License, Version 1.1
 *
 * Copyright (c) 2000 The Apache Software Foundation.  All rights
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:
 *       "This product includes software developed by the
 *        Apache Software Foundation (http://www.apache.org/)."
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 *
 * 4. The names "Apache" and "Apache Software Foundation" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact apache@apache.org.
 *
 * 5. Products derived from this software may not be called "Apache",
 *    nor may "Apache" appear in their name, without prior written
 *    permission of the Apache Software Foundation.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE APACHE SOFTWARE FOUNDATION OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Apache Software Foundation.  For more
 * information on the Apache Software Foundation, please see
 * <http://www.apache.org/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "apr_thread_proc.h"
#include "apr_errno.h"
#include "apr_general.h"
#include "apr_lib.h"

#define STRLEN 15

int main(int argc, char *argv[])
{
    ap_pool_t *context;

    ap_procattr_t *attr1 = NULL;
    ap_procattr_t *attr2 = NULL;
    ap_proc_t proc1;
    ap_proc_t proc2;
    ap_status_t s1;
    ap_status_t s2;
    char *args[2];

    fprintf(stdout, "Initializing.........");
    if (ap_initialize() != APR_SUCCESS) {
        fprintf(stderr, "Something went wrong\n");
        exit(-1);
    }
    fprintf(stdout, "OK\n");
    atexit(ap_terminate);

    fprintf(stdout, "Creating context.......");
    if (ap_create_pool(&context, NULL) != APR_SUCCESS) {
        fprintf(stderr, "Could not create context\n");
        exit(-1);
    }
    fprintf(stdout, "OK\n");

    fprintf(stdout, "This test relies on the process test working.  Please\n");
    fprintf(stdout, "run that test first, and only run this test when it\n");
    fprintf(stdout, "completes successfully.  Alternatively, you could run\n");
    fprintf(stdout, "server and client by yourself.\n");

    fprintf(stdout, "Creating children to run network tests.......\n");
    s1 = ap_createprocattr_init(&attr1, context);
    s2 = ap_createprocattr_init(&attr2, context);

    if (s1 != APR_SUCCESS || s2 != APR_SUCCESS) {
        fprintf(stderr, "Problem creating proc attrs\n");
        exit(-1);
    }

    args[0] = ap_pstrdup(context, "server");
    args[1] = NULL; 
    s1 = ap_create_process(&proc1, "./server", args, NULL, attr1, context);

    args[0] = ap_pstrdup(context, "client");
    s2 = ap_create_process(&proc2, "./client", args, NULL, attr2, context);

    if (s1 != APR_SUCCESS || s2 != APR_SUCCESS) {
        fprintf(stderr, "Problem spawning new process\n");
        exit(-1);
    }

    while ((s1 = ap_wait_proc(&proc1, APR_NOWAIT)) != APR_CHILD_DONE || 
           (s2 = ap_wait_proc(&proc2, APR_NOWAIT)) != APR_CHILD_DONE) {
        continue;
    }

    if (s1 == APR_SUCCESS) {
        ap_kill(&proc2, SIGTERM);
        ap_wait_proc(&proc2, APR_WAIT);
    }
    else {
        ap_kill(&proc1, SIGTERM);
        ap_wait_proc(&proc1, APR_WAIT);
    }
    fprintf(stdout, "Network test completed.\n");   

    return 1;
}
