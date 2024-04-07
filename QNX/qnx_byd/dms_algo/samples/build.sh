if [ -e target ]
then
    rm -rf target
fi

mkdir target && cd target && cmake .. && make -j 8 && make install && cd .. && rm -rf target
if [ ! $? -eq 0 ]; then
    exit 1
else
    exit 0
fi