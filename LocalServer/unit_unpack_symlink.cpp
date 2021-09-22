#include "myoption.hpp"

using namespace std;

int main(int argc, char **argv)
{
  int fd = open("symlink_pack", O_CREAT | O_RDWR);
  
  mkdir("unpack",00700);
  unpackOne("unpack",fd);

  close(fd);

  return 0;
}