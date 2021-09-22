## **注意后台运行的工作目录为默认目录，目前传参为相对路径时没有问题，绝对路径还没测试**   

### int copyAll(char* source, char* dest) 
> 复制 
*参数source 为要备份的原路径，参数dest为在目的路径（在备份目录下）  
*返回值 0 成功， -1 错误  

### int packAll(const char *currentPath, const char *targetFile)  
> 打包
*参数currentPath 为要备份文件的路径（可为任意文件）， 参数targetFile为打包目的文件的路径  
*返回值 0 成功， -1 错误  

### int unpackAll(const char *targetPath, const char *sourceFile)  
> 解包
*参数targetPath 是要解包的目的文件的路径（可为任意文件）， 参数sourceFile是源文件包（一个普通regular文件）的路径  
*返回值 0 成功， -1 错误  



***********  
以下未实现,实现思路都是基于打包、解包： 将所有文件打包成一个文件 --- 对文件压缩 --- (对文件加密) --- (对文件解密） --- 对文件解压缩 --- 将文件解包
### int compressAll(const char *currentPath, const char *targetFile)  
> 压缩
*参数currentPath 为要压缩文件的路径（可为任意文件）， 参数targetFile为压缩目的文件的路径  
*返回值 0 成功， -1 错误  
  
### int uncompressAll(const char *targetPath, const char *sourceFile)  
> 解压缩
*参数targetPath 是要解压缩的目的文件的路径（可为任意文件）， 参数sourceFile是源文件（一个普通regular文件）的路径  
*返回值 0 成功， -1 错误  


### int encrypAll(const char *currentPath, const char *targetFile)    
> 
*参数currentPath 为要加密文件的路径（可为任意文件）， 参数targetFile为加密目的文件的路径  
*返回值 0 成功， -1 错误  

### int decrypAll(const char *targetPath, const char *sourceFile)    
*参数targetPath 是要加密的目的文件的路径（可为任意文件）， 参数sourceFile是源文件包（一个普通regular文件）的路径  
*返回值 0 成功， -1 错误  

{
  errcode: 0 / 1
}

### 当前工作目录 getPWD

{
  errcode: 0/1,
  pwd: ''
}

### 目录下的文件 getCurrentLS
参数： pwd： 
{
  errcode: 0/1,
  trees: ''
}