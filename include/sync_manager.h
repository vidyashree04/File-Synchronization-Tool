

#ifndef SYNC_MANAGER_H

#define SYNC_MANAGER_H

void sync_create_or_update(const char *src_path, const char *dst_path);
void sync_delete(const char *dst_path);

#endif
