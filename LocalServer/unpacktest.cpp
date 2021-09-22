#include "myoption.hpp"

using namespace std;

int main(int argc, char **argv)
{
  int fd = open("pack1", O_CREAT | O_RDWR);
  
  mkdir("unpack",00700);
  while(unpackOne("unpack",fd) == SUCCESS_M ) ;

  close(fd);

  return 0;
}