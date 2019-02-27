#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/epoll.h>
using namespace std;
int getNumber(int a = 1){
	return a;
}
// int main()
// {
// 	string res;
// 	struct stat fileInfo;
// 	stat("../test.txt", &fileInfo);
// 	int fileFd = open("../test.txt", O_RDONLY);
// 	cout << "fileFd = " << fileFd << endl;
// 	char buf[100];
// 	read(fileFd, buf, 100);
// 	cout << "buf = " << buf << endl;
//     void *mmapRet = mmap(NULL, fileInfo.st_size, PROT_READ, MAP_PRIVATE, fileFd, 0);
// 	//close(fileFd);
//     if(mmapRet == MAP_FAILED)
// 	{
// 		munmap(mmapRet, fileInfo.st_size);
// 		cout << "failed!" << endl;
// 		return -1;
// 	}
//     char *src_addr = static_cast<char*>(mmapRet);
// 	//读取映射段
//     res += string(src_addr, src_addr + fileInfo.st_size);
//     cout << res << endl;
//     munmap(mmapRet, fileInfo.st_size);
//     int events = EPOLLIN | EPOLLOUT;
//     cout << EPOLLIN << "\t" << EPOLLOUT << endl;
//     cout << events << endl;
// 	return 0;
// }