#include "hclib.h"
int ____num_tasks[32] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
/**********************************************************************************************/
/*  This program is part of the Barcelona OpenMP Tasks Suite                                  */
/*  Copyright (C) 2009 Barcelona Supercomputing Center - Centro Nacional de Supercomputacion  */
/*  Copyright (C) 2009 Universitat Politecnica de Catalunya                                   */
/*                                                                                            */
/*  This program is free software; you can redistribute it and/or modify                      */
/*  it under the terms of the GNU General Public License as published by                      */
/*  the Free Software Foundation; either version 2 of the License, or                         */
/*  (at your option) any later version.                                                       */
/*                                                                                            */
/*  This program is distributed in the hope that it will be useful,                           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of                            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                             */
/*  GNU General Public License for more details.                                              */
/*                                                                                            */
/*  You should have received a copy of the GNU General Public License                         */
/*  along with this program; if not, write to the Free Software                               */
/*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA            */
/**********************************************************************************************/

#include "bots.h"
#include "fib.h"

#define FIB_RESULTS_PRE 41
long long fib_results[FIB_RESULTS_PRE] = {0,1,1,2,3,5,8,13,21,34,55,89,144,233,377,610,987,1597,2584,4181,6765,10946,17711,28657,46368,75025,121393,196418,317811,514229,832040,1346269,2178309,3524578,5702887,9227465,14930352,24157817,39088169,63245986,102334155};

long long fib_seq (int n)
{
	int x, y;
	if (n < 2) return n;

	x = fib_seq(n - 1);
	y = fib_seq(n - 2);

	return x + y;
}


long long fib (int n)
{
	long long x, y;
	if (n < 2) return n;

#ifdef HCLIB_TASK_UNTIED
#pragma omp task  shared(x) firstprivate(n) untied
#else
#pragma omp task  shared(x) firstprivate(n)
#endif
;
	{ ____num_tasks[omp_get_thread_num()]++;
x = fib(n - 1) ; }
;
#ifdef HCLIB_TASK_UNTIED
#pragma omp task  shared(y) firstprivate(n) untied
#else
#pragma omp task  shared(y) firstprivate(n)
#endif
;
	{ ____num_tasks[omp_get_thread_num()]++;
y = fib(n - 2) ; }
;

#pragma omp taskwait
;
	return x + y;
}


static long long par_res, seq_res;

void fib0 (int n)
{
{
#pragma omp parallel
;
        {
#pragma omp single
;
            {
                par_res = fib(n);
            }
        }
    } ; {
    int __i;
    assert(omp_get_max_threads() <= 32);
    for (__i = 0; __i < omp_get_max_threads(); __i++) {
        fprintf(stderr, "Thread %d: %d\n", __i, ____num_tasks[__i]);
    }
}

    bots_message("Fibonacci result for %d is %lld\n",n,par_res);
}

void fib0_seq (int n)
{
	seq_res = fib_seq(n);
	bots_message("Fibonacci result for %d is %lld\n",n,seq_res);
}

long long fib_verify_value(int n)
{
	if (n < FIB_RESULTS_PRE) return fib_results[n];
	return ( fib_verify_value(n-1) + fib_verify_value(n-2));
}

int fib_verify (int n)
{
	int result;

	if (bots_sequential_flag)
	{
		if (par_res == seq_res) result = BOTS_RESULT_SUCCESSFUL;
		else result = BOTS_RESULT_UNSUCCESSFUL;
	}
	else
	{
		seq_res = fib_verify_value(n);
		if (par_res == seq_res) result = BOTS_RESULT_SUCCESSFUL;
		else result = BOTS_RESULT_UNSUCCESSFUL;
	}

	return result;
}
