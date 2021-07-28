#!/usr/bin/perl

$num_args = $#ARGV + 1;
if ($num_args < 1) {
  print "\nUsage: perl kernel_extract.pl cuda_filename\n";
  exit;
}

$kernel=$ARGV[0];
$cudafilename="${kernel}.cu";
$kernelfilename="kernel_${kernel}.cu";
$pre_kernelfilename="pre_${kernel}.cu";
$post_kernelfilename="post_${kernel}.cu";

$DEBUG=0;

if ($DEBUG) {
  print "$cudafilename\n";
  print "$kernelfilename\n";
  print "$pre_kernelfilename\n";
  print "$post_kernelfilename\n";
}


open(cuda_file, $cudafilename); # preprocessed cuda file  
open(pre_kernel_file, ">${pre_kernelfilename}"); # everything before kernel goes here 
open(post_kernel_file, ">${post_kernelfilename}"); # everything after kernel goes here
open(kernel_file, ">${kernelfilename}"); # extracted kernel goes here


my(@lines) = <cuda_file>; # read file into list
my($line);

$foundKernel = 0;
$postcode = 0;
$done = 0;

$opencnt = 0;
$closecnt = 0;

foreach $line (@lines) {

  $_ = $line;

  # if we haven't encountered the kernel yet, check for __global__
  if (!$foundKernel) {
    ($word) = (m/^\s*(\w+)/x);
    $foundGlobal = $word eq __global__;
    $foundDevice = $word eq __device__;
    $foundKernel = $foundGlobal || $foundDevice;
  }

  # found the kernel 
  if ($foundKernel) {
  
    # if we haven't reached the end of the kernel, count braces in this line
    if (!$postcode) {
      $foundmatch = (m/{ | }/x);
      while ($foundmatch) {
	if ($& eq "}") {
	  $opencnt = $opencnt + 1;
	  if ($DEBUG) {
	    print $opencnt;
	  }
	}
	else { 
	  $closecnt = $closecnt + 1;
	  if ($DEBUG) {
	    print $closecnt;
	  }
	}
	# end of the kernel is reached when braces match
	if ($opencnt == $closecnt) {
	  $done = 1;
	  last;  
	}
	else {
	  $_ = $';
	  $foundmatch = (m/\{ + \}/x);
	}
      }
    }

    
    # finished counting braces for this line 

    # three possible scenarios
    
    # just reached the end of the kernel 
    if ($done) {
      print kernel_file $line;
      $postcode = 1;
      $done = 0;
    }
    # reached end of kernel in previous iteration
    # start post-processing 
    elsif ($postcode){ 
      print post_kernel_file $line;
    }
    # inside kernel 
    else {
      print kernel_file $line;
    }
  }
  # haven't seen the beginning of the kernel yet 
  else {
    print pre_kernel_file  $line 
  }
}


close(cuda_file);
close(pre_kernel_file);
close(post_kernel_file);
close(kernel_file);
