mkdir build
cd build
rm -r *
cmake -DBOOST_INCLUDEDIR=$BOOST_I -DBOOST_LIBRARYDIR=$BOOST_L ..
make
echo "Moving binary executable to project's root directory..."
mv hiddenchest* ..
#cp $HOME/mkx/build/hiddenchest* /home/edwin/workspace/MKXP/EkuipSkillsXP/ES
#echo "Searching for an executable file..."
#echo "Stripping binary executable now..."
#sleep 5
#exec strip ./hiddenchest.bin
#strip ./hiddenchest.binx64
#strip hiddenchest.binx64
if [ -x mkx.b ]; then #pplus.binx64
    echo "Found a 64 bit binary file!"
    strip ./mkx.b #pplus.binx64
elif [ -x hiddenchest.bin ]; then
    strip hiddenchest.bin
fi
echo "Happy Game Testing!"
