/*
 * Copyright 1993-2006 NVIDIA Corporation.  All rights reserved.
 *
 * NOTICE TO USER:   
 *
 * This source code is subject to NVIDIA ownership rights under U.S. and 
 * international Copyright laws.  
 *
 * NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE 
 * CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR 
 * IMPLIED WARRANTY OF ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH 
 * REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF 
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.   
 * IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL, 
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS 
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE 
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE 
 * OR PERFORMANCE OF THIS SOURCE CODE.  
 *
 * U.S. Government End Users.  This source code is a "commercial item" as 
 * that term is defined at 48 C.F.R. 2.101 (OCT 1995), consisting  of 
 * "commercial computer software" and "commercial computer software 
 * documentation" as such terms are used in 48 C.F.R. 12.212 (SEPT 1995) 
 * and is provided to the U.S. Government only as a commercial end item.  
 * Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through 
 * 227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the 
 * source code with only those rights set forth herein.
 */
 
 /*
	This file contains simple wrapper functions that call the CUDA kernels
 */

#include <cutil_inline.h>
#include <cstdlib>
#include <cstdio>
#include <string.h>
#include <GL/glew.h>
#include <cuda_gl_interop.h>

#include "particles_kernel.cu"
#include "ParticleSystem.cuh"

extern "C"
{

cudaArray *noiseArray;

void initCuda()
{
    cudaSetDevice( cutGetMaxGflopsDeviceId() );   
}

void setParameters(SimParams *hostParams)
{
    // copy parameters to constant memory
    cutilSafeCall( cudaMemcpyToSymbol(params, hostParams, sizeof(SimParams)) );
}

//Round a / b to nearest higher integer value
int iDivUp(int a, int b){
    return (a % b != 0) ? (a / b + 1) : (a / b);
}

// compute grid and thread block size for a given number of elements
void computeGridSize(int n, int blockSize, int &numBlocks, int &numThreads)
{
    numThreads = min(blockSize, n);
    numBlocks = iDivUp(n, numThreads);
}

inline float frand()
{
    return rand() / (float) RAND_MAX;
}

// create 3D texture containing random values
void createNoiseTexture(int w, int h, int d)
{
    cudaExtent size = make_cudaExtent(w, h, d);
    uint elements = (uint) size.width*size.height*size.depth;

    float *volumeData = new float [elements*4];
    float *ptr = volumeData;
    for(uint i=0; i<elements; i++) {
        *ptr++ = frand()*2.0f-1.0f;
        *ptr++ = frand()*2.0f-1.0f;
        *ptr++ = frand()*2.0f-1.0f;
        *ptr++ = frand()*2.0f-1.0f;
    }


    cudaChannelFormatDesc channelDesc = cudaCreateChannelDesc<float4>();
    cutilSafeCall( cudaMalloc3DArray(&noiseArray, &channelDesc, size) );

    cudaMemcpy3DParms copyParams = { 0 };
    copyParams.srcPtr   = make_cudaPitchedPtr((void*)volumeData, size.width*sizeof(float4), size.width, size.height);
    copyParams.dstArray = noiseArray;
    copyParams.extent   = size;
    copyParams.kind     = cudaMemcpyHostToDevice;
    cutilSafeCall( cudaMemcpy3D(&copyParams) );

    // set texture parameters
    noiseTex.normalized = true;                      // access with normalized texture coordinates
    noiseTex.filterMode = cudaFilterModeLinear;      // linear interpolation
    noiseTex.addressMode[0] = cudaAddressModeWrap;   // wrap texture coordinates
    noiseTex.addressMode[1] = cudaAddressModeWrap;
    noiseTex.addressMode[2] = cudaAddressModeWrap;

    // bind array to 3D texture
    cutilSafeCall(cudaBindTextureToArray(noiseTex, noiseArray, channelDesc));
}

void 
integrateSystem(float4 *oldPos, float4 *newPos,
				float4 *oldVel, float4 *newVel,
                float deltaTime,
                int numParticles)
{
    int numThreads, numBlocks;
    computeGridSize(numParticles, 256, numBlocks, numThreads);

    // execute the kernel
    integrateD<<< numBlocks, numThreads >>>(newPos, newVel,
                                            oldPos, oldVel,
                                            deltaTime,
											numParticles);
    
    // check if kernel invocation generated an error
    cutilCheckMsg("Kernel execution failed");
}


void 
calcDepth(float4*  pos, 
		  float*   keys,		// output
          uint*    indices,		// output 
          float3   sortVector,
          int      numParticles)
{
    int numThreads, numBlocks;
    computeGridSize(numParticles, 256, numBlocks, numThreads);

    // execute the kernel
    calcDepthD<<< numBlocks, numThreads >>>(pos, keys, indices, sortVector, numParticles);
    cutilCheckMsg("calcDepthD execution failed");
}

}   // extern "C"
