## **注意后台运行的工作目录为默认目录，目前传参为相对路径时没有问题，绝对路径还没测试**   

### int copyAll(char* source, char* target) 
> 复制 
*参数source 为要备份的原路径，参数dest为在目的路径（在备份目录下）  
*返回值 0 成功， 1 错误  
*输入：  
{  
  "method" : "copy",  
  "source" : "path1",  
  "target" : "path2"  
}  
#返回：  
{
  "errcode" : "0" / "1"
}



### int packAll(const char *source, const char *target)  
> 打包
*参数currentPath 为要备份文件的路径（可为任意文件）， 参数targetFile为打包目的文件的路径  
*返回值 0 成功， 1 错误  

*输入：  
{
  "method" : "pack" ,
  "source" : "path1" ,
  "target" : "targetfile"
}
#返回：  
{
  "errcode" : "0" / "1"
}



### int unpackAll(const char *targetPath, const char *sourceFile)  
> 解包
*参数targetPath 是要解包的目的文件的路径（可为任意文件）， 参数sourceFile是源文件（一个普通regular文件） 
*返回值 0 成功， 1 错误  

*输入：  
{
  "method" : "unpack" ,
  "source" : "sourceFile" ,
  "target" : "targetPath"
}
#返回：  
{
  "errcode" : "0" / "1"
}



### int compressAll(const char *currentPath, const char *targetFile)  
> 压缩
*参数currentPath 为要压缩文件， 参数targetFile为压缩目的文件
*返回值 0 成功， 1 错误  
*输入：
{
  "method" : "compress" ,
  "source" : "path1" ,
  "target" : "file1"
}
#返回：  
{
  "errcode" : "0" / "1"
}


  
### int uncompressAll(const char *targetPath, const char *sourceFile)  
> 解压缩
*参数targetPath 是要解压缩的目的文件， 参数sourceFile是源文件
*返回值 0 成功， 1 错误  
*输入：
{
  "method" : "uncompress" ,
  "source" : "file1" ,
  "target" : "path1"
}
#返回：  
{
  "errcode" : "0" / "1"
}



### int encrypAll(const char *currentfile, const char *targetFile)    
> 
*参数currentPath 为要加密文件名， 参数targetFile为加密目的文件
*返回值 0 成功， 1 错误  
*输入：
{
  "method" : "encrypt" ,
  "source" : "file1" ,
  "target" : "file2" 
}
#返回：  
{
  "errcode" : "0" / "1"
}



### int decrypAll(const char *targetPath, const char *sourceFile)    
*参数targetPath 是要解密的目的文件， 参数sourceFile是源文件
*返回值 0 成功， 1 错误  
*输入：
{
  "method" : "encrypt" ,
  "source" : "file1" ,
  "target" : "file2" 
}
#返回：  
{
  "errcode" : "0" / "1"
}

### 当前工作目录 getPWD
*输入
{
  method : getPWD
}
*返回

{
  errcode: 0/1,
  pwd : ""
}

### 目录下的文件 getCurrentLS
*输入
{
  method : getCurrnetLS,
  path : filepath
}
*返回
{
  errcode: 0/1,
  trees: [
      {"filename" : "" , "filetype" : "reg/dir/symlink/fifo" , "inode" : 5 ,"size" : 4567 },
      {"filename" : "" , "filetype" : "" , "inode" : 8 ,"size" : "" },
      ...
  ]
}
