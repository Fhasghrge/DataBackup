#include "myoption.hpp"

using namespace std;

int main(int argc, char **argv)
{
  /* 硬链接inode结点表记录 */
  map<ino_t, string> inodeTable;
/*
  int ret;
  if( (ret = copyAll("dir1","copyDir1", inodeTable)) < 0)
  {
    fprintf(stderr, "func copyAll test err\n");
    return -1;
  }
*/
  int fd = open("fifo_pack", O_CREAT | O_RDWR | O_TRUNC, 00600);
  packAll("fifo_test1", fd, inodeTable); 
  close(fd);

  return 0;
}