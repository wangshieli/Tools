#include "md5.h"

using std::ios;

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("��Ҫ�������ļ�·��\n");
		return 0;
	}

	string s_md5 = MD5(ifstream(argv[1], ios::binary)).toString();
	printf("md5 : %s\n", s_md5.c_str());

	FILE* fmd5 = NULL;
	fopen_s(&fmd5, "D:\\md5.txt", "wb");
	if (NULL == fmd5)
	{
		printf("��md5.txtʧ��\n");
		return 0;
	}

	fwrite(s_md5.c_str(), s_md5.size(), 1, fmd5);
	fclose(fmd5);

	return 0;
}