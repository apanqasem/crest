README file for CREST 0.1

### Description 

CREST is a tool to analyze dependencies in parallel GPU threads and perform source-level restructuring to obtain GPU kernels with varying
thread granularity.


### Contacts

   * Apan Qasem          apan@txstate.edu
   * Christopher Shaltz      cs1629@txstate.edu

Prior Contributors
    Swapneela Unkule



### Building and installing

Building from within crest/src/ directory

```
make
```

To build and install to ~/bin/

```
make install
```

### Example usage

#### Single-kernel workflow

Get a clean working copy of CUDA file
```
cp testsuite/simpleGL_kernel.cu .
```

Run the controlling script (example coarsening factor 2, block size 16)
```
sh bin/kernel_extract.sh simpleGL_kernel.cu 2 16
```
Transformed file is named `simpleGL_kernel.trans.cu`


#### Multi-kernel workflow

Get a clean copy of CUDA file
```
cp testsuite/transpose_kernel.cu .
```
Run the controlling script (example coarsening factor 2, block size 16)
```
sh bin/kernel_extract_multi.sh transpose_kernel.cu 2 16
```
Transformed file is named `transpose_kernel.trans.cu`


#### Multi-kernel, per-kernel, workflow:

Get a clean copy of CUDA file
```
cp testsuite/transpose_kernel.cu .
```
Extract CUDA kernels from mixed source (.cu extention is implied)
```
perl bin/kernel_extract_multi.pl transpose_kernel
```
Transform kernels with distinct coarsening factors and block sizes
```
   bin/crest transpose_kernel.cu.1k 2 8 > transpose_kernel.cu.1k.trans
   bin/crest transpose_kernel.cu.3k 4 8 > transpose_kernel.cu.3k.trans
```
Combine transformed kernels with unmodified non-kernel code
```
cat transpose_kernel.cu.0c \
    transpose_kernel.cu.1k.trans \
    transpose_kernel.cu.2c \
    transpose_kernel.cu.3k.trans \
    transpose_kernel.cu.4c \
  > transpose_kernel.trans.cu
```
Clean up intermediary files
```
rm transpose_kernel.cu.*
```

Supported command line options:
    Transformations and Analysis:
    -C          Suppress thread-coarsening transformations
                    (this may change to enabler in future)
    -F          Feature Extraction (under development, writes to "FV.txt")
    -R          Estimate register usage
    
    Utility:
    -h          Display some help / usage information and exit
    -l N        Line numbers start at N
    
    Debugging:
    -T          Display parse tree after transformations
	
    Reserved:
    -c    -d    -o    -S


Language features not supported or not yet implemented:
    Keywords extern, inline, register
        Only implemented in a few cases.
    Assignment chains
        e.g.  a = b = c = d;
    Preprocessor directives
    #pragma directives
    Pointers to user-defined types
    Pointer dereferencing for assignment targets
        e.g. *ptr = x;
    Empty statement blocks (i.e. { } )
    Empty statement (i.e. stand-alone ;)
    :: operator (C++ scope resolution operator)


### Known issues and limitations

   * Floating point literal formats have not been fully tested.
        Constants like 1.0f are attested in SDK example files and
            are accepted and pretty-printed correctly.
        Scientific notation ( e.g. 1.0E3 ) is tentative.
        Unary plus / minus does not always parse correctly.
   *  Hexadecimal constants beyond single-byte values have not been tested.
   *  Estimating register usage option is buggy due to frontend changes.
   *  Parser has difficulty with some forms / uses of user-defined data types.
   *  C++ templated types are problematic.
    


