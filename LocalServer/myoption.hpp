#ifndef my_option_hpp
#define my_option_hpp

//#include <websocketpp/config/asio_no_tls.hpp>
//#include <websocketpp/server.hpp>
#include <iostream>
#include <string>
#include <cstdio>
#include <cstring>
#include <climits>
#include <cstdlib>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include <map>
#include <utility>
#include <iterator>

#include <openssl/aes.h>

#define MAXLINK 5
#define MAXFILENUM 10
#define HEAD_LENGTH 16

/* 设置备份文件根目录 */
char backup[255] = "/home/byk/backup";

#define FALSE_M -1
#define SUCCESS_M 1

struct fileInfo{
  uint16_t pathLength;
  uint16_t isHardLink;
  uint32_t dataLength;
  mode_t mode;
  uid_t uid;
  gid_t gid;
};

typedef struct fileInfo* pFileInfo;

int writeInfo2File(int targetFd, pFileInfo head);
int readFromAtoB(int fd1, int fd2);
int readFileInfo(int sourceFd, pFileInfo head);
int readNDataFromAtoB(int fd1, int fd2, int size);

int copyAll(const char *currentPath, const char *targetPath, std::map<ino_t, std::string> &inodeTable)
{
  char backupPathbuf[64];
  char pathbuf[64];
  char CWD[64];


  /* 错误验证处理 */


  /* 如果要备份的文件目录不是绝对路径，增加前缀 /home/byk/backup 修改为绝对路径 */
  if(targetPath[0] != '/')
    snprintf(backupPathbuf, 64, "%s/%s", backup, targetPath);
  else
    strncpy(backupPathbuf, targetPath, 64);

  /* 如果当前操作路径不是绝对路径，增加前缀 currentWorkingDir 修改为绝对路径 */
  if(currentPath[0] != '/')
    snprintf(pathbuf, 64, "%s/%s", getcwd(CWD, 64), currentPath);
  else
    strncpy(pathbuf, currentPath, 64);

  struct stat statbuf;
  struct timespec times[2];
  DIR *dp;
  int ret, n, fd1, fd2;
  uid_t owner;
  gid_t group;
  //mode_t mode;

  /* 得到文件的当前信息 */
  if(lstat(pathbuf, &statbuf) < 0)
  {
    fprintf(stderr, "lstat error: %s\n", pathbuf);
    return -1;
  }

  /* 保存当前文件的inode,若已经存在，说明是硬链接 */
  auto iter = inodeTable.find(statbuf.st_ino);
  if(S_ISDIR(statbuf.st_mode) == 0 && iter != inodeTable.end())
  {
    if( (ret = link(iter->second.c_str(), backupPathbuf)) < 0)
    {
      fprintf(stderr, "link error: from %s to %s\n", iter->second.c_str(), backupPathbuf);
      return -1;
    }
    return SUCCESS_M;
  }
  else if(S_ISDIR(statbuf.st_mode) == 0)
  {
    inodeTable.insert(std::pair<ino_t, std::string>(statbuf.st_ino, backupPathbuf));
  }

  /* 备份需要的系统文件信息 */
  times[0].tv_sec = statbuf.st_atime;
  //times[0].tv_nsec = statbuf.st_atime.tv_nsec;
  times[1].tv_sec = statbuf.st_mtime;
  //times[1].tv_nsec = statbuf.st_mtime.tv_nsec;

  owner = statbuf.st_uid;
  group = statbuf.st_gid;
  //mode = statbuf.st_mode;

  /* 如果不是目录，将文件复制到相应的地方 */
  /* 如果是普通文件 */
  if(S_ISREG(statbuf.st_mode))
  {
    if( (fd1 = open(pathbuf, O_RDONLY)) < 0)
    {
      fprintf(stderr, "open error: %s\n", pathbuf);
      return -1;
    }

    if( (fd2 = open(backupPathbuf, O_WRONLY | O_CREAT | O_TRUNC, statbuf.st_mode)) == -1)
    {
      fprintf(stderr, "open error: %s\n", backupPathbuf);
      return -1;      
    }

    if( (ret = readFromAtoB(fd1, fd2)) == FALSE_M )
    {
      fprintf(stderr, "copy error: from %s to %s\n", pathbuf, backupPathbuf);
      return -1;   
    }

#if 0
    /* 修改文件的系统信息 */
    if(futimens(fd1,times) < 0)
    {
      fprintf(stderr, "futimes error: %s\n", pathbuf);
      return -1;
    }

    if(futimens(fd2,times) < 0)
    {
      fprintf(stderr, "futimes error: %s\n", backupPathbuf);
      return -1;
    }
#endif

    /* 修改文件所有者 */
    if( (ret = fchown(fd2, owner, group)) == -1)
    {
      fprintf(stderr, "chown error: %s\n", backupPathbuf);
      return -1;
    }

    /* 关闭文件 */
    close(fd2);
    close(fd1);
  }

  /* 如果是管道文件 */
  if(S_ISFIFO(statbuf.st_mode))
  {
    if(mkfifo(backupPathbuf, statbuf.st_mode) < 0)
    {
      fprintf(stderr, "mkfifo error: %s\n", backupPathbuf);
      return -1;
    }

    /* 修改文件所有者 */
    if( (ret = chown(backupPathbuf, owner, group)) == -1)
    {
      fprintf(stderr, "chown error: %s\n", backupPathbuf);
      return -1;
    }
  }

  /* 如果是符号链接 */
  if(S_ISLNK(statbuf.st_mode))
  {
    char linkbuf[64];
    char linkfilePath[64];
    getcwd(CWD, 64);
    if(readlink(pathbuf, linkbuf, 64) < 0)
    {
      fprintf(stderr, "readlink error: %s\n", pathbuf);
      return -1;
    }

    /* 处理得到backupPathbuf的上级目录路径，目的是和readlink得到的相对路径结合 */
    int n = strlen(backupPathbuf);
    char backupDIR[64];
    strncpy(backupDIR, backupPathbuf, 64);
    while(backupDIR[n-1] != '/' && n>0)
    {
      backupDIR[n-1] = '\0';
      n--;
    }
    /* 得到备份链接文件在备份目录树中的绝对路径，这里有bug，没有处理readlink得到绝对路径的情况 */
    if(linkbuf[0] != '/')
      snprintf(linkfilePath, 64, "%s%s", backupDIR, linkbuf);
    else
      return -1;
    
    if(symlink(linkfilePath, backupPathbuf) < 0)
    {
      fprintf(stderr, "symlink error: %s\n", backupPathbuf);
      return -1;
    }

    /* 修改文件所有者 */
    if( (ret = lchown(backupPathbuf, owner, group)) == -1)
    {
      fprintf(stderr, "chown error: %s\n", backupPathbuf);
      return -1;
    }
  }

  /* 如果是 目录文件，需要处理循环 */
  if(S_ISDIR(statbuf.st_mode))
  {
    struct dirent *dirp;
    DIR *dp;
    char childFilePath[64];
    char backupChildFilePath[64];

    if( (dp = opendir(pathbuf)) == NULL)
    {
      fprintf(stderr, "opendir error: %s\n", pathbuf);
      return -1;
    }

    /* 创建备份目录文件 */
    if( (ret = mkdir(backupPathbuf, statbuf.st_mode)) < 0 )
    {
      fprintf(stderr, "mkdir error: %s\n", backupPathbuf);
      return -1;
    }

    /* 修改文件所有者 */
    if( (ret = chown(backupPathbuf, owner, group)) == -1)
    {
      fprintf(stderr, "chown error: %s\n", backupPathbuf);
      return -1;
    }

    while( (dirp = readdir(dp)) != NULL)
    {
      if(strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
      {
        continue;
      }

      snprintf(childFilePath, 64, "%s/%s", pathbuf, dirp->d_name);
      snprintf(backupChildFilePath, 64, "%s/%s", backupPathbuf, dirp->d_name);
      if(copyAll(childFilePath, backupChildFilePath, inodeTable) < 0)
      {
        fprintf(stderr, "copy %s to %s error\n", childFilePath, backupChildFilePath);
        return FALSE_M;
      }
    }
  }

  return SUCCESS_M;
}

int packAll(const char *currentPath, int targetFd, std::map<ino_t, std::string> &inodeTable)
{
  char pathbuf[64];
  char CWD[64];

  /* 错误验证处理 */

  /* 如果当前操作路径不是绝对路径，增加前缀 currentWorkingDir 修改为绝对路径 */
  if(currentPath[0] != '/')
    snprintf(pathbuf, 64, "%s/%s", getcwd(CWD, 64), currentPath);
  else
    strncpy(pathbuf, currentPath, 64);

  struct stat statbuf;
  DIR *dp;
  int ret, fd1;
  pFileInfo head = (struct fileInfo *)malloc(sizeof(struct fileInfo));

  /* 得到文件的当前信息 */
  if(lstat(pathbuf, &statbuf) < 0)
  {
    fprintf(stderr, "lstat error: %s\n", pathbuf);
    return -1;
  }

  head->pathLength = strlen(pathbuf)+1;
  head->mode = statbuf.st_mode;
  head->uid = statbuf.st_uid;
  head->gid = statbuf.st_gid;
  head->isHardLink = 0;

  /* 保存当前文件的inode,若已经存在，说明是硬链接 */
  auto iter = inodeTable.find(statbuf.st_ino);
  if(S_ISDIR(statbuf.st_mode) == 0 && iter != inodeTable.end())
  {
    head->isHardLink = 1;
    head->dataLength = strlen(iter->second.c_str()) +1;    /* 硬链接的data字段是第一个文件的绝对路径 */

    /* 保存fileInfo */    
    if(writeInfo2File(targetFd, head) == FALSE_M)
    {
      fprintf(stderr, "writeInfo2File error: %s\n", pathbuf);
      return -1;
    }

    /* 保存 path */
    if( (ret = write(targetFd, pathbuf, head->pathLength)) == -1 )
    {
      fprintf(stderr, "write hardLink path error: form %s to %s\n", pathbuf, iter->second.c_str());
      return -1;
    }

    /* 保存 data (硬链接指向的Inode结点的第一个路径) */
    if( (ret = write(targetFd, iter->second.c_str(), head->dataLength)) == -1 )
    {
      fprintf(stderr, "write hardLink path error: form %s to %s\n", pathbuf, iter->second.c_str());
      return -1;
    }

    free(head);
    return SUCCESS_M;
  }
  else if(S_ISDIR(statbuf.st_mode) == 0)
  {
    head->isHardLink = 0;
    inodeTable.insert(std::pair<ino_t, std::string>(statbuf.st_ino, pathbuf));
  }  

  /* 如果不是目录，将文件复制到相应的地方 */
  /* 如果是普通文件 */
  if(S_ISREG(statbuf.st_mode))
  {
    if( (fd1 = open(pathbuf, O_RDONLY)) < 0)
    {
      fprintf(stderr, "open error: %s\n", pathbuf);
      return -1;
    }

    /* 保存文件的长度，之后再将偏移量调整到文件起始处 */
    if( (head->dataLength = lseek(fd1, 0, SEEK_END)) == -1)
    {
      fprintf(stderr, "lseek SEEK_END error: %s\n", pathbuf);
      return -1;
    }
    if(lseek(fd1, 0, SEEK_SET) == -1)
    {
      fprintf(stderr, "lseek SEEK_SET error: %s\n", pathbuf);
      return -1;
    }

    /* 保存fileInfo */
    if(writeInfo2File(targetFd, head) == FALSE_M)
    {
      fprintf(stderr, "writeInfo2FIle error: %s\n", pathbuf);
      return -1;
    }

    /* 保存 path */
    if( (ret = write(targetFd, pathbuf, head->pathLength)) == -1 )
    {
      fprintf(stderr, "write symlink path error: %s\n", pathbuf);
      return -1;
    }

    /* 保存文件数据 */
    if( (ret = readFromAtoB(fd1, targetFd)) == FALSE_M )
    {
      fprintf(stderr, "copy error: serialize from %s\n", pathbuf);
      return -1;   
    }

    /* 验证文件数据大小 */
    if(ret != head->dataLength)
    {
      fprintf(stderr, "read data size error: file %s\n", pathbuf);
      return -1;
    }

    /* 关闭文件 */
    close(fd1);
  }

  /* 如果是管道文件 */
  if(S_ISFIFO(statbuf.st_mode))
  {
    head->dataLength = 0;

    /* 保存fileInfo */
    if(writeInfo2File(targetFd, head) == FALSE_M)
    {
      fprintf(stderr, "writeInfo2FIle error: %s\n", pathbuf);
      return -1;
    }

    /* 保存 path */
    if( (ret = write(targetFd, pathbuf, head->pathLength)) == -1 )
    {
      fprintf(stderr, "write symlink path error: %s\n", pathbuf);
      return -1;
    }
  }

  /* 如果是符号链接 */
  if(S_ISLNK(statbuf.st_mode))
  {
    char linkbuf[64];
    char linkfilePath[64];
    int ret;

    if( (ret = readlink(pathbuf, linkbuf, 64)) < 0)
    {
      fprintf(stderr, "readlink error: %s\n", pathbuf);
      return -1;
    }

    linkbuf[ret] = '\0';

    snprintf(linkfilePath, 64, "%s", linkbuf);
    head->dataLength = strlen(linkfilePath) +1;

    /* 保存fileInfo */
    if(writeInfo2File(targetFd, head) == FALSE_M)
    {
      fprintf(stderr, "writeInfo2FIle error: %s\n", pathbuf);
      return -1;
    }

    /* 保存 path */
    if( (ret = write(targetFd, pathbuf, head->pathLength)) == -1 )
    {
      fprintf(stderr, "write symlink path error: %s\n", pathbuf);
      return -1;
    }

    /* 保存symlink data */
    if( (ret = write(targetFd, linkfilePath, head->dataLength)) == -1 )
    {
      fprintf(stderr, "write symlink data error: %s\n", pathbuf);
      return -1;
    }

  }

  /* 如果是 目录文件，需要处理循环 */
  if(S_ISDIR(statbuf.st_mode))
  {
    struct dirent *dirp;
    DIR *dp;
    char childFilePath[64];

    head->dataLength = 0;
    if( (dp = opendir(pathbuf)) == NULL)
    {
      fprintf(stderr, "opendir error: %s\n", pathbuf);
      return -1;
    }

    /* 保存fileInfo */
    if(writeInfo2File(targetFd, head) == FALSE_M)
    {
      fprintf(stderr, "writeInfo2FIle error: %s\n", pathbuf);
      return -1;
    }

    /* 保存 path */
    if( (ret = write(targetFd, pathbuf, head->pathLength)) == -1 )
    {
      fprintf(stderr, "write symlink path error: %s\n", pathbuf);
      return -1;
    }

    while( (dirp = readdir(dp)) != NULL)
    {
      if(strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
      {
        continue;
      }

      snprintf(childFilePath, 64, "%s/%s", pathbuf, dirp->d_name);
      if(packAll(childFilePath, targetFd, inodeTable) < 0)
      {
        fprintf(stderr, "pack error: file %s\n", childFilePath);
        return FALSE_M;
      }
    }
  }

  free(head);
  return SUCCESS_M;
}

int unpackOne(const char *targetPath, int sourceFd)
{
  char backupPathbuf[64];
  char CWD[64];
  char newPath[64];

  /* 错误验证处理 */

  DIR *dp;
  int ret, fd1;
  pFileInfo head = (struct fileInfo *)malloc(sizeof(struct fileInfo));

  /* 读取FileInfo */
  if( (ret = readFileInfo(sourceFd, head)) == FALSE_M)
  {
    fprintf(stderr, "readFileInfo error\n");
    return -1;
  }

  /* 读取Path */
  if( (ret = read(sourceFd, backupPathbuf, head->pathLength)) != head->pathLength)
  {
    fprintf(stderr, "read Path error\n");
    return -1;
  }

  /* 还原到指定的路径 */
  if(targetPath != NULL)
  {
    /* 得到相对路径 pPath*/
    getcwd(CWD, 64);
    int num = strlen(CWD);
    char *pPath = backupPathbuf;

    memmove(pPath, pPath + num, strlen(pPath) + 1 - num);

    /* 在相对路径前加个前缀 targetPath, 如果targetPath是绝对路径，向后扩充相对路径；否则加当前路径及前缀为绝对路径 */
    if(targetPath[0] == '/')
    {
      snprintf(newPath, 64, "%s%s",targetPath, backupPathbuf);
    }
    else
    {
      snprintf(newPath, 64, "%s/%s%s", CWD, targetPath, backupPathbuf);
    }
  }
  else /* 还原到原来的路径 */
  {
    strncpy(newPath, backupPathbuf, 64);
  }

  /* 如果是硬链接，读数据 (原目录树中绝对路径) */
  if(head->isHardLink == 1)
  {
    char hardPathBuf[64];
    char newHardPath[64];

    if( (ret = read(sourceFd, hardPathBuf, head->dataLength)) == -1 )
    {
      fprintf(stderr, "read actual hardpath in unpackOne error: %s\n", newPath);
      return FALSE_M;
    }

    /* 还原硬链接到指定的路径 */
    if(targetPath != NULL)
    {
      /* 得到相对路径 pPath*/
      getcwd(CWD, 64);
      int num = strlen(CWD);
      char *pPath = hardPathBuf;

      memmove(pPath, pPath + num, strlen(pPath) + 1 - num);

      /* 在相对路径前加个前缀 targetPath, 如果targetPath是绝对路径，向后扩充相对路径；否则加当前路径及前缀为绝对路径 */
      if(targetPath[0] == '/')
      {
        snprintf(newHardPath, 64, "%s%s",targetPath, hardPathBuf);
      }
      else
      {
        snprintf(newHardPath, 64, "%s/%s%s", CWD, targetPath, hardPathBuf);
      }
    }
    else /* 还原到原来的路径 */
    {
      strncpy(newHardPath, hardPathBuf, 64);
    }

    /* 创建硬链接 */
    if( (ret = link(newHardPath, newPath)) == -1)
    {
      fprintf(stderr, "symlink in unPackOne error: %s\n", newPath);
      return FALSE_M;
    }

    free(head);
    return SUCCESS_M;
  }

  /* 如果是管道文件或者目录，不必读取数据 */
  if(S_ISDIR(head->mode))
  {
    if(mkdir(newPath, head->mode) == -1)
    {
      fprintf(stderr, "mkdir error: %s\n", newPath);
      return FALSE_M;
    }
  }

  if(S_ISFIFO(head->mode))
  {
    if(mkfifo(newPath, head->mode) == -1)
    {
      fprintf(stderr, "mkfifo error: %s\n",newPath);
      return FALSE_M;
    }
  }

  /* 如果是普通文件，需要读数据 */
  if(S_ISREG(head->mode))
  {
    if( (fd1 = open(newPath, O_WRONLY | O_CREAT | O_TRUNC, head->mode)) == -1)
    {
      fprintf(stderr, "open_creat error: %s\n", newPath);
      return FALSE_M;
    }

    if( (ret = readNDataFromAtoB(sourceFd, fd1, head->dataLength)) == FALSE_M)
    {
      fprintf(stderr, "readNData error: %s\n", newPath);
      return FALSE_M;
    }

    if( (ret = close(fd1)) == FALSE_M)
    {
      fprintf(stderr, "close error: %s\n", newPath);
      return FALSE_M;
    }
  }

  /* 如果是符号链接，读数据（symlink 需要更改路径） */
  if(S_ISLNK(head->mode))
  {
    char symlinkbuf[64];

    if( (ret = read(sourceFd, symlinkbuf, head->dataLength)) != head->dataLength )
    {
      fprintf(stderr, "read symlink in unpackOne error: %s\n", newPath);
      return FALSE_M;
    }

    if( (ret = symlink(symlinkbuf, newPath)) == -1)
    {
      fprintf(stderr, "symlink in unPackOne error: %s\n", newPath);
      return FALSE_M;
    }
  }

  /* 修改文件所有者 */
  if( (ret = lchown(newPath, head->uid, head->gid)) == -1)
  {
    fprintf(stderr, "chown error: %s\n", newPath);
    return -1;
  }

  free(head);
  return SUCCESS_M;
}

int copyOne(const char *path1, const char *path2)
{


  return SUCCESS_M;
}

static int funcOne(const char *FullPath1, const struct stat *statptr ,const char *FullPath2)
{
  struct stat statbuf;
  
  return SUCCESS_M;
}

int readFromAtoB(int fd1, int fd2)
{
  int n = -1, count = 0;
  char buf[4096];

  while( (n = read(fd1, buf, 4096)) > 0 )
  {
    if(write(fd2, buf, n) != n)
    {
      fprintf(stderr, "write error\n");
      return FALSE_M;
    }
    count += n;
  }

  return count;
}

int writeInfo2File(int targetFd, pFileInfo head)
{
  /* 分别写入 pathlength isHardLink dataLength mode uid gid */
  int ret;
  if( (ret = write(targetFd, &head->pathLength, sizeof(head->pathLength))) == -1 )
  {
    fprintf(stderr, "write error\n");
    return FALSE_M;    
  }

  if( (ret = write(targetFd, &head->isHardLink, sizeof(head->isHardLink))) == -1 )
  {
    fprintf(stderr, "write error\n");
    return FALSE_M;    
  }

  if( (ret = write(targetFd, &head->dataLength, sizeof(head->dataLength))) == -1 )
  {
    fprintf(stderr, "write error\n");
    return FALSE_M;    
  }

  if( (ret = write(targetFd, &head->mode, sizeof(head->mode))) == -1 )
  {
    fprintf(stderr, "write error\n");
    return FALSE_M;    
  }

  if( (ret = write(targetFd, &head->uid, sizeof(head->uid))) == -1 )
  {
    fprintf(stderr, "write error\n");
    return FALSE_M;    
  }

  if( (ret = write(targetFd, &head->gid, sizeof(head->gid))) == -1 )
  {
    fprintf(stderr, "write error\n");
    return FALSE_M;    
  }

  return SUCCESS_M;   
}

int readFileInfo(int sourceFd, pFileInfo head)
{
  /* 分别读入 pathlength isHardLink dataLength mode uid gid */
  int ret;
  if( (ret = read(sourceFd, &head->pathLength, sizeof(head->pathLength))) == -1 )
  {
    fprintf(stderr, "read error\n");
    return FALSE_M;    
  }

  if( (ret = read(sourceFd, &head->isHardLink, sizeof(head->isHardLink))) == -1 )
  {
    fprintf(stderr, "read error\n");
    return FALSE_M;    
  }

  if( (ret = read(sourceFd, &head->dataLength, sizeof(head->dataLength))) == -1 )
  {
    fprintf(stderr, "read error\n");
    return FALSE_M;    
  }

  if( (ret = read(sourceFd, &head->mode, sizeof(head->mode))) == -1 )
  {
    fprintf(stderr, "read error\n");
    return FALSE_M;    
  }

  if( (ret = read(sourceFd, &head->uid, sizeof(head->uid))) == -1 )
  {
    fprintf(stderr, "read error\n");
    return FALSE_M;    
  }

  if( (ret = read(sourceFd, &head->gid, sizeof(head->gid))) == -1 )
  {
    fprintf(stderr, "read error\n");
    return FALSE_M;    
  }

  return SUCCESS_M;
}

int readNDataFromAtoB(int fd1, int fd2, int size)
{
  int n = -1, count = 0;
  char buf[4096];
  int block, sigchar;
  block = size / 4096;
  sigchar = size % 4096;

  while( count < block && (n = read(fd1, buf, 4096)) > 0 )
  {
    if(write(fd2, buf, n) != n)
    {
      fprintf(stderr, "write block error\n");
      return FALSE_M;
    }
    count += 1;
  }

  if( (n = read(fd1, buf, sigchar)) > 0 )
  {
    if(write(fd2, buf, n) != n)
    {
      fprintf(stderr, "write sigchar error\n");
      return FALSE_M;
    }
  }

  return SUCCESS_M;
}

#endif