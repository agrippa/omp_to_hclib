#include "hclib.h"
int ____num_tasks[32] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "backprop.h"
#include "omp.h"

extern char *strcpy();
extern void exit();

int layer_size = 0;

void backprop_face()
{
  BPNN *net;
  int i;
  net = bpnn_create(layer_size, 16, 1); // (16, 1 can not be changed)
  printf("Input layer size : %d\n", layer_size);
  load(net);
  //entering the training kernel, only one iteration
  printf("Starting training kernel\n");
{
    float out_err, hid_err;
    bpnn_train_kernel(net, &out_err, &hid_err);
  } ; {
    int __i;
    assert(omp_get_max_threads() <= 32);
    for (__i = 0; __i < omp_get_max_threads(); __i++) {
        fprintf(stderr, "Thread %d: %d\n", __i, ____num_tasks[__i]);
    }
}

  bpnn_free(net);
  printf("Training done\n");
}

int setup(int argc, char *argv[])
{
  if(argc!=2){
  fprintf(stderr, "usage: backprop <num of input elements>\n");
  exit(0);
  }

  layer_size = atoi(argv[1]);
  
  int seed;

  seed = 7;   
  bpnn_initialize(seed);
  backprop_face();

  exit(0);
}