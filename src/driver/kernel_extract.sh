#!/bin/sh

if [ $# -lt 1 ]; then 
    echo "usage: "
    echo "      kernel_extract.sh filename"
    echo 
    exit
fi

# look for crest

CREST=`which crest`

if [ ! $CREST ]; then 
    CREST="$HOME/crest/bin/crest"
fi

kernel=`basename $1 .cu`

GCC=gcc

if [ ${DEBUG} ]; then 
    echo $@
fi


#
# Extract kernel
#
echo "Extracting kernel ..."
perl $HOME/crest/bin/kernel_extract.pl $kernel

#
# Apply transformation
#
echo "Applying transformations ..."
${CREST} kernel_${kernel}.cu > ${kernel}.trans.cu

#
# cat everything back
#
echo "Restoring file ..."
cat pre_${kernel}.cu ${kernel}.trans.cu post_${kernel}.cu > tmp.cu
mv tmp.cu ${kernel}.trans.cu

rm -rf  pre_${kernel}.cu  post_${kernel}.cu kernel_${kernel}.cu
