#include "myoption.hpp"
#include "AES.hpp"

int main()
{
  int ret;
  if ( (ret = encrypt("pack1","pack1_encrypt", "123456")) != 0)
  {
    return -1;
  }

  if ( (ret = decrypt("pack1_encrypt","pack1_encrypt_result","123456")) != 0)
  {
    return -1;
  }

  return 0;
}