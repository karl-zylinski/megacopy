#include <stdio.h>
#include <conio.h>
#include <dir.h>
#include <sys/stat.h>

void create_folders(char* path)
{
	char buf[128];
	int i = 3;
	char c;
	printf(path);
	while(1)
	{
		c = path[i];
		if(c == '\\' || c == '\0')
		{
			memcpy(buf, path, i);
			buf[i] = '\0';
			printf("Ensuring folder %s exists.\n", buf);
			mkdir(buf);
		}
		if(c == '\0')
		{
			break;
		}
		++i;
	}
}

int copy_file(char* folder, char* source, char* dest)
{
	char command[256];
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
	if(dest_stat.st_size != source_stat.st_size)
	{
		strcpy(command, "del ");
		strcat(command, dest);
		system(command);
		return 1;
	}
	return 0;
}

void copy_files(char* path)
{
	int done;
	char retry_response;
	char searchpath[128];
	char subpath[128];
	char destpath[128];
	char destfolder[128];
	struct ffblk a;
	strcpy(searchpath, path);
	strcat(searchpath, "\\*.*");
	done = findfirst(searchpath, &a, DRIVE);

	while(!done)
	{
		if(strcmp(a.ff_name, "..") == 0 || strcmp(a.ff_name, ".") == 0)
		{
			done = findnext(&a);
			continue;
		}

		strcpy(subpath, path);
		strcat(subpath, "\\");
		strcat(subpath, a.ff_name);
		if(chdir(subpath) == 0)
		{
			copy_files(subpath);
		}
		else
		{
			strcpy(destpath, subpath);
			destpath[0] = 'a';
			strcpy(destfolder, path);
			destfolder[0] = 'a';
			while(copy_file(destfolder, subpath, destpath) != 0)
			{
				printf("Failed copy of %s. Press s to skip, q to quit. Any other key retries.", subpath);
				retry_response = getch();
				if(retry_response == 's')
				{
					break;
				}
				if(retry_response == 'q')
				{
					return;
				}
			}
		}
		done = findnext(&a);
	}
}

int main()
{
	copy_files("c:");
	printf("Done.\n");
	return 0;
}