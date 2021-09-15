#ifndef my_option_hpp
#define my_option_hpp

//#include <websocketpp/config/asio_no_tls.hpp>
//#include <websocketpp/server.hpp>
#include <iostream>
#include <string>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

/* 设置备份文件根目录 */
char backup[255] = "/home/byk/backup";

#define FALSE_M -1
#define SUCCESS_M 1

int readFromAtoB(int fd1, int fd2);

int copyAll(const char *currentPath, const char *targetPath)
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
      if(copyAll(childFilePath, backupChildFilePath) < 0)
      {
        fprintf(stderr, "copy %s to %s error\n", childFilePath, backupChildFilePath);
        return FALSE_M;
      }
    }
  }

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
  int n=-1;
  char buf[4096];

  while( (n = read(fd1, buf, 4096)) > 0 )
  {
    if(write(fd2, buf, n) != n)
    {
      fprintf(stderr, "write error\n");
      return FALSE_M;
    }
  }

  return SUCCESS_M;
}




#endif