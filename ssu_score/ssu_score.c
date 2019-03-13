#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

void directoryopen(char *);

int main(int argc, char *argv[])
{

   DIR            *dir_info, *student_dir;
   struct dirent  *dir_entry, *student_entry;


   dir_info = opendir(argv[1]);              // 현재 디렉토리를 열기
   if ( NULL != dir_info)
   {
      while( dir_entry   = readdir( dir_info))  // 디렉토리 안에 있는 모든 파일과 디렉토리 출력
      {
         printf( "%s\n", dir_entry->d_name);
      }
      closedir( dir_info);
   }
}

