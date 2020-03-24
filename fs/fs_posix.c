#include <fs/fs_posix.h>


/*
+------------------------------------------------------------------------------
| Function    : open
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
| Returns     :
|
+------------------------------------------------------------------------------
*/
int fs_open(const char *file, int flags, int mode)
{
	int fd;

	fd = open(file, flags, mode);
	return fd;
}

/*
+------------------------------------------------------------------------------
| Function    : close
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
| Returns     :
|
+------------------------------------------------------------------------------
*/
int fs_close(int fd)
{

	return close(fd);
}

/*
+------------------------------------------------------------------------------
| Function    : read
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
| Returns     :
|
+------------------------------------------------------------------------------
*/
int fs_read(int fd, void *buf, int len)
{

	return read(fd, buf, len);
}

/*
+------------------------------------------------------------------------------
| Function    : write
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
| Returns     :
|
+------------------------------------------------------------------------------
*/
int fs_write(int fd, const void *buf, int len)
{

	return write(fd, buf, len);
}


/*
+------------------------------------------------------------------------------
| Function    : lseek
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
| Returns     :
|
+------------------------------------------------------------------------------
*/
int fs_lseek(int fd, int offset, int dir)
{

	return lseek(fd, offset, dir);
}


/*
+------------------------------------------------------------------------------
| Function    : unlink
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
| Returns     :
|
+------------------------------------------------------------------------------
*/
int fs_unlink(const char *pathname)
{

	return unlink(pathname);
}


/*
+------------------------------------------------------------------------------
| Function    : stat
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
| Returns     :
|
+------------------------------------------------------------------------------
*/
int fs_stat(const char *file, struct stat *buf)
{

	return stat(file, buf);
}


/*
+------------------------------------------------------------------------------
| Function    : mkdir
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
| Returns     :
|
+------------------------------------------------------------------------------
*/
int fs_mkdir(const char *path, rt_uint16_t mode)
{

	return mkdir(path, mode);
}	

/*
+------------------------------------------------------------------------------
| Function    : rmdir
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
| Returns     :
|
+------------------------------------------------------------------------------
*/
int fs_rmdir(const char *pathname)
{

	return rmdir(pathname);
}


/*
+------------------------------------------------------------------------------
| Function    : opendir
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
| Returns     :
|
+------------------------------------------------------------------------------
*/
DIR_POSIX* fs_opendir(const char* name)
{

	return opendir(name);
}

/*
+------------------------------------------------------------------------------
| Function    : readdir
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
| Returns     :
|
+------------------------------------------------------------------------------
*/
struct dirent* fs_readdir(DIR_POSIX *d)
{

	return readdir(d);
}

/*
+------------------------------------------------------------------------------
| Function    : telldir
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
| Returns     :
|
+------------------------------------------------------------------------------
*/
rt_off_t fs_telldir(DIR_POSIX *d)
{

	return telldir(d);
}

/*
+------------------------------------------------------------------------------
| Function    : seekdir
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
| Returns     :
|
+------------------------------------------------------------------------------
*/
void fs_seekdir(DIR_POSIX *d, rt_off_t offset)
{

	seekdir(d, offset);
}

/*
+------------------------------------------------------------------------------
| Function    : rewinddir
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
| Returns     :
|
+------------------------------------------------------------------------------
*/
void fs_rewinddir(DIR_POSIX *d)
{

	rewinddir(d);
}

/*
+------------------------------------------------------------------------------
| Function    : closedir
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
| Returns     :
|
+------------------------------------------------------------------------------
*/
int fs_closedir(DIR_POSIX* d)
{

	return closedir(d);
}

/*
+------------------------------------------------------------------------------
| Function    : chdir
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
| Returns     :
|
+------------------------------------------------------------------------------
*/
#ifdef DFS_USING_WORKDIR
int fs_chdir(const char *path)
{

	return chdir(path);
}
#endif


void fs_DeleteDir(const char *directory)
{
	DIR_POSIX* dirp;
	struct dirent* entry;
	struct stat s;
	char line_buffer[32];

	dirp = fs_opendir(directory);
	if (dirp == NULL)
		return;
	while (1)
	{
		entry = fs_readdir(dirp);
		if (entry == NULL)
			break;
		rt_sprintf(line_buffer, "%s/%s", directory, entry->d_name);
		fs_stat(line_buffer, &s);
		if (s.st_mode & S_IFDIR)
			fs_DeleteDir(line_buffer);
		fs_unlink(line_buffer);
	}
	fs_closedir(dirp);
}




