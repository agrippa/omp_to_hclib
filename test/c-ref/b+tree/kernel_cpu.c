#include "hclib.h"
#ifdef __cplusplus
#include "hclib_cpp.h"
#include "hclib_system.h"
#ifdef __CUDACC__
#include "hclib_cuda.h"
#endif
#endif
// #ifdef __cplusplus
// extern "C" {
// #endif

//========================================================================================================================================================================================================200
//	DEFINE/INCLUDE
//========================================================================================================================================================================================================200

//======================================================================================================================================================150
//	LIBRARIES
//======================================================================================================================================================150

#include <omp.h>									// (in directory known to compiler)			needed by openmp
#include <stdlib.h>									// (in directory known to compiler)			needed by malloc
#include <stdio.h>									// (in directory known to compiler)			needed by printf, stderr
#include <assert.h>

//======================================================================================================================================================150
//	COMMON
//======================================================================================================================================================150

#include "common.h"								// (in directory provided here)

//======================================================================================================================================================150
//	UTILITIES
//======================================================================================================================================================150

#include "timer.h"					// (in directory provided here)

//========================================================================================================================================================================================================200
//	KERNEL_CPU FUNCTION
//========================================================================================================================================================================================================200

typedef struct _pragma93_omp_parallel {
    int thid;
    int bid;
    int i;
    long long (*time0_ptr);
    long long (*time1_ptr);
    long long (*time2_ptr);
    int (*threadsPerBlock_ptr);
    int (*cores_arg_ptr);
    record (*(*records_ptr));
    knode (*(*knodes_ptr));
    long (*knodes_elem_ptr);
    int (*order_ptr);
    long (*maxheight_ptr);
    int (*count_ptr);
    long (*(*currKnode_ptr));
    long (*(*offset_ptr));
    int (*(*keys_ptr));
    record (*(*ans_ptr));
 } pragma93_omp_parallel;


#ifdef OMP_TO_HCLIB_ENABLE_GPU

class pragma93_omp_parallel_hclib_async {
    private:
        __device__ int hclib_get_current_worker() {
            return blockIdx.x * blockDim.x + threadIdx.x;
        }

    int i;
    volatile long maxheight;
    int thid;
    volatile int threadsPerBlock;
    knode* volatile knodes;
    long* volatile currKnode;
    int bid;
    int* volatile keys;
    long* volatile offset;
    volatile long knodes_elem;
    record* volatile ans;
    record* volatile records;

    public:
        pragma93_omp_parallel_hclib_async(int set_i,
                long set_maxheight,
                int set_thid,
                int set_threadsPerBlock,
                knode* set_knodes,
                long* set_currKnode,
                int set_bid,
                int* set_keys,
                long* set_offset,
                long set_knodes_elem,
                record* set_ans,
                record* set_records) {
            i = set_i;
            maxheight = set_maxheight;
            thid = set_thid;
            threadsPerBlock = set_threadsPerBlock;
            knodes = set_knodes;
            currKnode = set_currKnode;
            bid = set_bid;
            keys = set_keys;
            offset = set_offset;
            knodes_elem = set_knodes_elem;
            ans = set_ans;
            records = set_records;

        }

        __device__ void operator()(int bid) {
            {

		// process levels of the tree
		for(i = 0; i < maxheight; i++){

			// process all leaves at each level
			for(thid = 0; thid < threadsPerBlock; thid++){

				// if value is between the two keys
				if((knodes[currKnode[bid]].keys[thid]) <= keys[bid] && (knodes[currKnode[bid]].keys[thid+1] > keys[bid])){
					// this conditional statement is inserted to avoid crush due to but in original code
					// "offset[bid]" calculated below that addresses knodes[] in the next iteration goes outside of its bounds cause segmentation fault
					// more specifically, values saved into knodes->indices in the main function are out of bounds of knodes that they address
					if(knodes[offset[bid]].indices[thid] < knodes_elem){
						offset[bid] = knodes[offset[bid]].indices[thid];
					}
				}

			}

			// set for next tree level
			currKnode[bid] = offset[bid];

		}

		//At this point, we have a candidate leaf node which may contain
		//the target record.  Check each key to hopefully find the record
		// process all leaves at each level
		for(thid = 0; thid < threadsPerBlock; thid++){

			if(knodes[currKnode[bid]].keys[thid] == keys[bid]){
				ans[bid].value = records[knodes[currKnode[bid]].indices[thid]].value;
			}

		}

	}
        }
};

#else
static void pragma93_omp_parallel_hclib_async(void *____arg, const int ___iter0);
#endif
void 
kernel_cpu(	int cores_arg,

			record *records,
			knode *knodes,
			long knodes_elem,

			int order,
			long maxheight,
			int count,

			long *currKnode,
			long *offset,
			int *keys,
			record *ans)
{

	//======================================================================================================================================================150
	//	Variables
	//======================================================================================================================================================150

	// timer
	long long time0;
	long long time1;
	long long time2;

	time0 = get_time();

	//======================================================================================================================================================150
	//	MCPU SETUP
	//======================================================================================================================================================150

	int threadsPerBlock;
	threadsPerBlock = order < 1024 ? order : 1024;

    {

	time1 = get_time();

	//======================================================================================================================================================150
	//	PROCESS INTERACTIONS
	//======================================================================================================================================================150

	// private thread IDs
	int thid;
	int bid;
	int i;

	// process number of querries
 { 
pragma93_omp_parallel *new_ctx = (pragma93_omp_parallel *)malloc(sizeof(pragma93_omp_parallel));
new_ctx->thid = thid;
new_ctx->bid = bid;
new_ctx->i = i;
new_ctx->time0_ptr = &(time0);
new_ctx->time1_ptr = &(time1);
new_ctx->time2_ptr = &(time2);
new_ctx->threadsPerBlock_ptr = &(threadsPerBlock);
new_ctx->cores_arg_ptr = &(cores_arg);
new_ctx->records_ptr = &(records);
new_ctx->knodes_ptr = &(knodes);
new_ctx->knodes_elem_ptr = &(knodes_elem);
new_ctx->order_ptr = &(order);
new_ctx->maxheight_ptr = &(maxheight);
new_ctx->count_ptr = &(count);
new_ctx->currKnode_ptr = &(currKnode);
new_ctx->offset_ptr = &(offset);
new_ctx->keys_ptr = &(keys);
new_ctx->ans_ptr = &(ans);
hclib_loop_domain_t domain[1];
domain[0].low = 0;
domain[0].high = count;
domain[0].stride = 1;
domain[0].tile = -1;
#ifdef OMP_TO_HCLIB_ENABLE_GPU
hclib::future_t *fut = hclib::forasync_cuda((count) - (0), pragma93_omp_parallel_hclib_async(i, maxheight, thid, threadsPerBlock, knodes, currKnode, bid, keys, offset, knodes_elem, ans, records), hclib::get_closest_gpu_locale(), NULL);
fut->wait();
#else
hclib_future_t *fut = hclib_forasync_future((void *)pragma93_omp_parallel_hclib_async, new_ctx, 1, domain, HCLIB_FORASYNC_MODE);
hclib_future_wait(fut);
#endif
free(new_ctx);
 } 

	time2 = get_time();
    }

	//======================================================================================================================================================150
	//	DISPLAY TIMING
	//======================================================================================================================================================150

	printf("Time spent in different stages of CPU/MCPU KERNEL:\n");

	printf("%15.12f s, %15.12f % : MCPU: SET DEVICE\n",					(float) (time1-time0) / 1000000, (float) (time1-time0) / (float) (time2-time0) * 100);
	printf("%15.12f s, %15.12f % : CPU/MCPU: KERNEL\n",					(float) (time2-time1) / 1000000, (float) (time2-time1) / (float) (time2-time0) * 100);

	printf("Total time:\n");
	printf("%.12f s\n", 												(float) (time2-time0) / 1000000);

} 

#ifndef OMP_TO_HCLIB_ENABLE_GPU

static void pragma93_omp_parallel_hclib_async(void *____arg, const int ___iter0) {
    pragma93_omp_parallel *ctx = (pragma93_omp_parallel *)____arg;
    int thid; thid = ctx->thid;
    int bid; bid = ctx->bid;
    int i; i = ctx->i;
    do {
    bid = ___iter0;
{

		// process levels of the tree
		for(i = 0; i < (*(ctx->maxheight_ptr)); i++){

			// process all leaves at each level
			for(thid = 0; thid < (*(ctx->threadsPerBlock_ptr)); thid++){

				// if value is between the two keys
				if(((*(ctx->knodes_ptr))[(*(ctx->currKnode_ptr))[bid]].keys[thid]) <= (*(ctx->keys_ptr))[bid] && ((*(ctx->knodes_ptr))[(*(ctx->currKnode_ptr))[bid]].keys[thid+1] > (*(ctx->keys_ptr))[bid])){
					// this conditional statement is inserted to avoid crush due to but in original code
					// "offset[bid]" calculated below that addresses knodes[] in the next iteration goes outside of its bounds cause segmentation fault
					// more specifically, values saved into knodes->indices in the main function are out of bounds of knodes that they address
					if((*(ctx->knodes_ptr))[(*(ctx->offset_ptr))[bid]].indices[thid] < (*(ctx->knodes_elem_ptr))){
						(*(ctx->offset_ptr))[bid] = (*(ctx->knodes_ptr))[(*(ctx->offset_ptr))[bid]].indices[thid];
					}
				}

			}

			// set for next tree level
			(*(ctx->currKnode_ptr))[bid] = (*(ctx->offset_ptr))[bid];

		}

		//At this point, we have a candidate leaf node which may contain
		//the target record.  Check each key to hopefully find the record
		// process all leaves at each level
		for(thid = 0; thid < (*(ctx->threadsPerBlock_ptr)); thid++){

			if((*(ctx->knodes_ptr))[(*(ctx->currKnode_ptr))[bid]].keys[thid] == (*(ctx->keys_ptr))[bid]){
				(*(ctx->ans_ptr))[bid].value = (*(ctx->records_ptr))[(*(ctx->knodes_ptr))[(*(ctx->currKnode_ptr))[bid]].indices[thid]].value;
			}

		}

	} ;     } while (0);
}

#endif


//========================================================================================================================================================================================================200
//	END
//========================================================================================================================================================================================================200

// #ifdef __cplusplus
// }
// #endif

