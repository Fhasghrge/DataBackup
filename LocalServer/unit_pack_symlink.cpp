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

  /* 打开的文件描述符是要写入的描述符 */
  int fd = open("symlink_pack", O_CREAT | O_RDWR | O_TRUNC, 00600);
  packAll("symlink_test1", fd, inodeTable); 
  close(fd);

  return 0;
}