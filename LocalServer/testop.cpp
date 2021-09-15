#include "myoption.hpp"

using namespace std;

int main(int argc, char **argv)
{
  char *buf = (char*)malloc(24*sizeof(char));
  if( (buf = getcwd(buf, 24)) == NULL)
  {
    printf("error\n");
    return -1;
  }
  printf("mode_t :%d\nuid_t :%d\ngid_t :%d\nstruct timespec :%d\n", sizeof(mode_t),
          sizeof(uid_t), sizeof(gid_t), sizeof(struct timespec));
  
  //open("file1" , O_CREAT | O_WRONLY )

  /* 硬链接inode结点表记录 */
  map<ino_t, string> inodeTable;

  int ret;
  if( (ret = copyAll("dir1","copyDir1", inodeTable)) < 0)
  {
    fprintf(stderr, "func copyAll test err\n");
    return -1;
  }


  return 0;
}