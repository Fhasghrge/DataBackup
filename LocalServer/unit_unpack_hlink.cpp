#include "myoption.hpp"

using namespace std;

int main(int argc, char **argv)
{
  int fd = open("hlink_pack", O_CREAT | O_RDWR);
  
  mkdir("unpack",00700);

  unpackOne("unpack",fd);
  unpackOne("unpack",fd);
  unpackOne("unpack",fd);

  close(fd);

  return 0;
}