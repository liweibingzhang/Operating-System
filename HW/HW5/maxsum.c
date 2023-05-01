// Elapsed Real Time for input-5.txt:
// Type of GPU:
#include <stdio.h>
#include <stdbool.h>
#include <cuda_runtime.h>
// Input sequence of values.
int *vList;
// Number of values on the list.
int vCount = 0;
// Capacity of the list of values.
int vCap = 0;
// General function to report a failure and exit.
static void fail(char const *message)
{
  fprintf(stderr, "%s\n", message);
  exit(1);
}
// Print out a usage message, then exit.
static void usage()
{
  printf("usage: maxsum [report]\n");
  exit(1);
}
// Read the list of values.
__host__ void readList()
{
  // Set up initial list and capacity.
  vCap = 5;
  vList = (int *)malloc(vCap * sizeof(int));

  // Keep reading as many values as we can.
  int v;
  while (scanf("%d", &v) == 1)
  {
    // Grow the list if needed.
    if (vCount >= vCap)
    {
      vCap *= 2;
      vList = (int *)realloc(vList, vCap * sizeof(int));
    }

    // Store the latest value in the next array slot.
    vList[vCount++] = v;
  }
}

__global__ void checkSum(int *results_dev, int *vList_dev, int vCount, bool report)
{
  int idx = blockDim.x * blockIdx.x + threadIdx.x;
  int local_max = -999;
  if (idx < vCount)
  {
    for (int i = idx; i < vCount; i++)
    {
      int sum = 0;
      for (int j = i; j < vCount; j++)
      {
        sum += vList_dev[j];
        if (sum > local_max)
        {
          local_max = sum;
        }
      }
    }
    results_dev[idx] = local_max;
    if (report)
    {
      printf("I'm thread %d. The maximum sum I found is %d.\n", idx, local_max);
    }
  }
}
  int main(int argc, char *argv[])
  {
    if (argc < 1 || argc > 2)
      usage();
    // If there's an argument, it better be "report"
    bool report = false;
    if (argc == 2)
    {
      if (strcmp(argv[1], "report") != 0)
        usage();
      report = true;
    }
    readList();
    // Add code to allocate memory on the device and copy over the list.
    int *vList_dev = NULL;
    int *results_dev = NULL;
    if (cudaMalloc((void **)&vList_dev, vCount * sizeof(int)) != cudaSuccess)
      fail("Failed to allocate space for lenght list on device");
    // Add code to copy the list over to the device.
    if (cudaMemcpy(vList_dev, vList, vCount * sizeof(int),
                   cudaMemcpyHostToDevice) != cudaSuccess)
      fail("Failed to copy list to device");
    // Add code to allocate space on the device to hold the results.
    if (cudaMalloc((void **)&results_dev, vCount * sizeof(int)) != cudaSuccess)
      fail("Failed to allocate space for results on device");
    // Block and grid dimensions.
    int threadsPerBlock = 100;
    // Round up for the number of blocks we need.
    int blocksPerGrid = (vCount + threadsPerBlock - 1) / threadsPerBlock;

    // Run our kernel on these block/grid dimensions (you'll need to add some parameters)
    checkSum<<<blocksPerGrid, threadsPerBlock>>>(results_dev, vList_dev, vCount, report);
    if (cudaGetLastError() != cudaSuccess)
      fail("Failure in CUDA kernel execution.");

    // Add code to copy results back to the host, compare the local largest products
    // and report the final largest product
    int *results_host = (int *)malloc(vCount * sizeof(int));
    if (cudaMemcpy(results_host, results_dev, vCount * sizeof(int),
                   cudaMemcpyDeviceToHost) != cudaSuccess)
      fail("Can't copy list from device to host");
    // Find the maximum sum in the results
    int maxSum = 0;
    for (int i = 0; i < vCount; i++)
    {
      if (results_host[i] > maxSum)
      {
        maxSum = results_host[i];
      }
    }
    printf("Maximum Sum: %d\n", maxSum);
    // Free memory on the device and the host.
    cudaFree(vList_dev);
    cudaFree(results_dev);
    free(vList);
    cudaDeviceReset();
    return 0;
  }