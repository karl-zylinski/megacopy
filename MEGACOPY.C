/*
	This application is for DOS. It should compile with Turbo C 2.01. It copies files
	from one place to another and lets you retry / skip / quit if the operation fails.
	It's very handy for copying a directory tree off a harddrive onto several floppys.

	Written by Karl Zylinski
	karl@zylinski.se
	http://zylinski.se

	This code is public domain.
*/

#include <stdio.h>
#include <conio.h>
#include <dir.h>
#include <sys/stat.h>
#define MAX_PATH_LEN 128

static void create_folders(char* path) {
	char buf[MAX_PATH_LEN];
	int i = 3;
	char c;
	printf("Ensuring folder %s exists.\n", path);
	while(1) {
		c = path[i];
		if(c == '\\' || c == '\0') {
			memcpy(buf, path, i);
			buf[i] = '\0';
			mkdir(buf);
		}
		if(c == '\0') {
			break;
		}
		++i;
	}
}

static int copy_file(char* folder, char* source, char* dest) {
	char command[6 + MAX_PATH_LEN * 2];
	struct stat source_stat;
	struct stat dest_stat;
	create_folders(folder);
	strcpy(command, "copy ");
	strcat(command, source);
	strcat(command, " ");
	strcat(command, dest);
	printf("Copying file %s to %s.\n", source, dest);
	system(command);
	stat(source, &source_stat);
	stat(dest, &dest_stat);
	if(dest_stat.st_size != source_stat.st_size) {
		strcpy(command, "del ");
		strcat(command, dest);
		system(command);
		return 1;
	}
	return 0;
}

static int is_dir(char* path) {
	struct stat dir_stat;
	stat(path, &dir_stat);
	return (dir_stat.st_mode & S_IFDIR) != 0;
}

static void copy_files(char* source_root, char* dest_root, char* subpath) {
	int done;
	char retry_response;
	char searchpath[MAX_PATH_LEN];
	char curpath[MAX_PATH_LEN];
	char curfolder[MAX_PATH_LEN];
	char destpath[MAX_PATH_LEN];
	char destfolder[MAX_PATH_LEN];
	struct ffblk file;
	strcpy(searchpath, source_root);
	strcat(searchpath, subpath);
	strcat(searchpath, "\\*.*");
	done = findfirst(searchpath, &file, DRIVE);
	while(!done) {
		if(strcmp(file.ff_name, "..") == 0 || strcmp(file.ff_name, ".") == 0) {
			done = findnext(&file);
			continue;
		}
		strcpy(curpath, source_root);
		strcat(curpath, subpath);
		strcat(curpath, "\\");
		strcat(curpath, file.ff_name);
		if(is_dir(curpath)) {
			strcpy(curfolder, subpath);
			strcat(curfolder, "\\");
			strcat(curfolder, file.ff_name);
			copy_files(source_root, dest_root, curfolder);
		} else {
			strcpy(destfolder, dest_root);
			strcat(destfolder, subpath);
			strcpy(destpath, destfolder);
			strcat(destpath, "\\");
			strcat(destpath, file.ff_name);
			while(copy_file(destfolder, curpath, destpath) != 0) {
				printf("Failed copy of %s. Press s to skip, q to quit. Any other key retries.\n", curpath);
				retry_response = getch();
				if(retry_response == 's') {
					break;
				}
				if(retry_response == 'q') {
					return;
				}
			}
		}
		done = findnext(&file);
	}
}

static char* usage_string = "Usage: megacopy.exe absolute_source_path absolute_destination_path.\n";

static char* strip_trailing_slash(char* path) {
	int len = strlen(path);
	if (path[len - 1] == '\\') {
		path[len - 1] = '\0';
	}
	return path;
}

static int str_begins_with(char* str, char* cmp) {
	int i;
	int slen = strlen(str);
	int cmplen = strlen(cmp);
	if (cmplen > slen) {
		return 0;
	}
	for (i = 0; i < cmplen; ++i) {
		if (cmp[i] != str[i]) {
			return 0;
		}
	}
	return 1;
}

int main (int argc, char *argv[]) {
	char source_dir[MAX_PATH_LEN];
	char dest_dir[MAX_PATH_LEN];
	if (argc != 3) {
		printf("Too few arguments.\n");
		printf(usage_string);
		return 1;
	}
	if (!is_dir(argv[1])) {
		printf("Source path does not exist or is not a directory.\n");
		printf(usage_string);
		return 1;
	}
	if (!is_dir(argv[2])) {
		printf("Destination path does not exist or is not a directory.\n");
		printf(usage_string);
		return 1;
	}
	strcpy(source_dir, argv[1]);
	strip_trailing_slash(source_dir);
	strcpy(dest_dir, argv[2]);
	strip_trailing_slash(dest_dir);
	if (str_begins_with(dest_dir, source_dir)) {
		printf("Destination directory is inside source directory. This is not allowed since it could lead to undefined behavior.\n");
		printf(usage_string);
		return 1;
	}
	copy_files(source_dir, dest_dir, "");
	printf("\nDone.\n");
	return 0;
}

