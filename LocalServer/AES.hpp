#ifndef aes_hpp
#define aes_hpp

/*
*参数描述：
*加密：第一个参数：需要加密的文件 第二个参数：加密后生成的二进制文件名 第三个参数：加密的秘钥
*解密：第一个参数：加密的文件 第二个参数：解密后生成的文件名 第三个参数：秘钥
*/
int encrypt(const char *sourceFile, const char *output, const char *user_key);
int decrypt(const char *input, const char *targetFile, const char *user_key);

int encrypt(const char *sourceFile, const char *output, const char *user_key)
{
  FILE* fp_plain =NULL;
  FILE* fp_encrypt = NULL;
  unsigned char plain_data[16],encrypt_data[16];
  int ret,file_size=0;
  AES_KEY key;
  const unsigned char *p_key = (const unsigned char*) user_key;

  AES_set_encrypt_key(p_key,128,&key);	

  fp_plain = fopen(sourceFile,"rb");
  if(fp_plain==NULL)
  {
    perror("fopen");
    return -1;
  }
  fp_encrypt = fopen(output,"wb");
  if(fp_encrypt==NULL)
  {
    perror("fopen");
    return -1;
  }
  
  while(ret=fread(plain_data,1,16,fp_plain))
  {
    AES_encrypt(plain_data,encrypt_data,&key);
    fwrite(encrypt_data,1,16,fp_encrypt);
    if(ret < 16) break;
  }
  
  fwrite(&ret,1,1,fp_encrypt);

  fclose(fp_plain);
  fclose(fp_encrypt);

  return 0;
}

int decrypt(const char *input, const char *targetFile, const char *user_key)
{
  FILE* fp_plain =NULL;
  FILE* fp_encrypt = NULL;
  unsigned char plain_data[16],encrypt_data[18];
  int ret;
  AES_KEY key;
  const unsigned char *p_key = (const unsigned char*) user_key;

  AES_set_decrypt_key(p_key,128,&key);	

  fp_encrypt = fopen(input,"rb");
  if(fp_encrypt==NULL)
  {
    perror("fopen");
    return -1;
  }
  
  fp_plain = fopen(targetFile,"wb");
  if(fp_plain==NULL)
  {
    perror("fopen");
    return -1;
  }
  
  while(ret=fread(encrypt_data,1,18,fp_encrypt))
  {
    AES_decrypt(encrypt_data,plain_data,&key);
    if(ret < 18)
    {	
      fwrite(plain_data,1,encrypt_data[16] ? encrypt_data[16]:16,fp_plain);
      break;
    }
    
    fwrite(plain_data,1,16,fp_plain);
  
      fseek(fp_encrypt,-2,SEEK_CUR);

  }
  
  fclose(fp_plain);
  fclose(fp_encrypt);

  return 0;
}

#endif