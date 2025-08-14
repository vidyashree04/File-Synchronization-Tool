

#include <stdio.h>
#include <sys/stat.h>
#include <utime.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "metadata_handler.h"

void preserve_metadata(const char *src_path, const char *dst_path) {
struct stat src_stat;
if (stat(src_path, &src_stat) != 0) {
perror("stat (metadata)");
return;
}


if (chmod(dst_path, src_stat.st_mode) != 0) {
perror("chmod failed");

}


struct utimbuf new_times;
new_times.actime = src_stat.st_atime; 
new_times.modtime = src_stat.st_mtime; 

if (utime(dst_path, &new_times) != 0) {
perror("utime failed");
}
}
