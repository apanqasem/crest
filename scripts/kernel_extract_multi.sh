#!/bin/sh
#
# script to process a CUDA source file with N kernels
#
# original script by Dr. Apan Qasem
# adapted for multi-kernel sources Fall 2012
#		by Christopher Shaltz cs1629@txstate.edu


if [ $# -lt 1 ]; then 
    echo "usage: "
    echo "      kernel_extract_multi.sh filename factor blocksize"
    echo 
    exit
fi

# look for crest
CREST=`which crest 2>/dev/null`

if [ ! $CREST ]; then 
    CREST="$HOME/crest/bin/crest"
fi

# gather command line args
kernel=`basename $1 .cu`
factor=$2
blockSize=$3

#
if [ ${DEBUG} ]; then 
    echo $@
fi

#
# Extract kernel(s)
#
echo "Extracting kernel(s) ..."
perl $HOME/crest/bin/kernel_extract_multi.pl $kernel
lineno=1
num=0


#
# Apply transformation
#
filelist=""
echo "Applying transformations ..."
status="continue"
while [ $status = "continue" ]; do
	odd=`expr $num % 2`
	if [ $odd -eq 1 ]; then
		ext="${num}k"
		filename=${kernel}.cu.${ext}
		#echo "file ${filename}"
		if [ -f ${filename} ]; then
			filelist="${filelist} ${filename}.trans"
			echo "Transforming '${filename}'..."
			${CREST} ${filename} ${factor} ${blockSize} -l ${lineno} > ${filename}.trans
		else
			status="done"
		fi
	else
		ext="${num}c"
		filename=${kernel}.cu.${ext}
		#echo "file ${filename}"
		if [ -f ${filename} ]; then
			filelist="${filelist} ${filename}"
		else
			status="done"
		fi
	fi

	# if we are not done, account for the 
	if [ $status = "continue" ]; then
		filelength=`wc -l "${filename}" | cut -d " " -f 1`
		lineno=$(expr $lineno + $filelength)
		num=$(expr $num + 1)
	fi
done

#
# cat everything back
#
echo "Restoring file ..."
cat ${filelist} > ${kernel}.trans.cu

# clean up temp files
echo "Cleaning up temporary files..."
rm -f ${kernel}.cu.[0-9]*

