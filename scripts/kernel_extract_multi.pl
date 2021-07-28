#!/usr/bin/perl
#
# kernel_extract_multi.pl
#
# extract all CUDA kernels from input file
#

use warnings;
use strict;

my($num_args) = $#ARGV + 1;
if ($num_args < 1) {
  print "\nUsage: perl kernel_extract_multi.pl cuda_filename\n";
  exit;
}

my($kernel)=$ARGV[0];
my($cudafilename)="${kernel}.cu";


# open cuda source file
open(cuda_file, $cudafilename) or die ("unable to open $cudafilename");

# one-up number for output files
my($num) = 0;

# output filenames
my(@files) = "$cudafilename.${num}c";
#print "$files[0]\n";

# open output file for the first inter-kernel segment
open(output_file, ">${files[0]}") or die ("unable to open output file.");

# read file into list
my(@lines) = <cuda_file>;
my($line);


my($mode) = 0;		# mode: 0=non-kernel, 1=kernel
my($opencount) = 0;		# counting curly braces
my($closecount) = 0;

my($word);
my($foundGlobal);
my($foundDevice);
my($foundKernel);
my($foundmatch);

foreach $line (@lines)
{
	$_ = $line;

	if ($mode == 0)
	{
		# looking for a kernel
		($word) = ( m/^\s*(\w+)/x ) ;
		if ($word)
		{
			$foundGlobal = $word eq "__global__";
			$foundDevice = $word eq "__device__";
			$foundKernel = $foundGlobal || $foundDevice;
		}
		else
		{
			$foundKernel = "";
		}

		if ($foundKernel)
		{
			#print $line;
			# we've found the start of a kernel, so shift direction of output
			close(output_file);
			++$num;
			$files[$num] = "$cudafilename.${num}k";
			open(output_file, ">${files[$num]}") or die ("unable to open output file.");
			#print output_file $line;

			# set mode to in-kernel and reset curly brace counts
			$mode = 1;
			$opencount = 0;
			$closecount = 0;
		}
		else
		{
			print output_file $line;
		}
	}
	
	# this is a separate if block so that we can immediately start counting curly braces
	if ($mode == 1)
	{
		print output_file $line;

		# looking for end of kernel (by counting curly braces)
		$_ = $line;
		$foundmatch = (m/{ | }/x);
		while (($foundmatch) && ($mode == 1))
		{
			if ($& eq "{")
			{
				$opencount = $opencount + 1;
				#print "{";
			}
			else
			{
				$closecount = $closecount + 1;
				#print "}";
			}

			# if the counts match, then this kernel is done
			if (($opencount > 0) && ($opencount == $closecount))
			{
				# shift direction of output
				close(output_file);
				++$num;
				$files[$num] = "$cudafilename.${num}c";
				open(output_file, ">${files[$num]}") or die ("unable to open output file.");

				# set mode to out-of-kernel
				$mode = 0;
			}
			else
			{
				$_ = $';
				$foundmatch = (m/\{ + \}/x);
			}
		}
	} # end if mode == 1

} # end for each line

close(cuda_file);
print "\n@files\n";

# end kernel_extract_multi.pl

