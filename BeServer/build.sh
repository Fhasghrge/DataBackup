mkdir dir1
date > dir1/file11
echo file12 > dir1/file12
ln -s ./dir2/file21 ./dir1/link21

mkdir ./dir1/dir2
mkfifo ./dir1/dir2/fifo1
echo file21 > ./dir1/dir2/file21
echo file22 > ./dir1/dir2/file22
ln ./dir1/file11 ./dir1/dir2/hlink21

g++ -std=c++11 -ljsoncpp -lcrypto -lpthread  -lboost_system -o server1 mypro.cpp

exit

