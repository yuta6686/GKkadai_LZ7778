// LZ78������
#include <stdio.h>			// printf �ŕK�v
#include <string.h>			// memcmp, strlen �ŕK�v

// �Ƃ肠�����Œ蒷�z��� (���p���l����Ɖϒ��̕K�v����)
#define MAX_DIC		100000	// �����ő吔
#define MAX_LZ78	100000	// �o�̓f�[�^�ő吔

struct DIC {				// ���� (�Q�Ɣԍ��͓Y��+1�Ƃ���)
	unsigned int	len;	// �f�[�^��
	unsigned char* data;	// �f�[�^�擪 (���̂͌��̃f�[�^)
} g_dic[MAX_DIC];
int	g_dcount = 0;			// �����o�^��

#pragma pack(1) // �p�f�B���O�i�]���f�[�^�̏���������)
struct LZ78 {				// LZ78�R�[�h
	unsigned int	index;	// �Q�Ɣԍ�
	unsigned char	data;	// �f�[�^
} g_lz78[MAX_LZ78];
#pragma pack()
int g_lzcount = 0;			// �o�̓R�[�h��

const char* FILE_NAME = "hal.bmp";
const char* COMPRESSION_FILE_NAME = "hal.lz";
const char* DECOMPRESSION_FILE_NAME = "hal_decomp.bmp";

int main(void)
{
	//char str[] = "ABCBCBCDEBCDABCD";	//�e�X�g�p������
	//int len = strlen(str);				//������̒���
	//unsigned char* data = (unsigned char*)str;//�������P�Ȃ�1�o�C�g�f�[�^�Ƃ��Ĉ���

	FILE* file;
	file = fopen(FILE_NAME, "rb");

	unsigned int len;
	fseek(file, 0, SEEK_END);
	len = ftell(file);
	fseek(file, 0, SEEK_SET);

	unsigned char* data;
	data = new unsigned char[len];

	fread(data, len, 1, file);

	fclose(file);


	//��f�[�^�������o�^
	g_dic[0].data = (unsigned char*)"";
	g_dic[0].len = 0;
	g_dcount = 1;

	g_lzcount = 0;

	int c = 0;

	while (c < len)
	{
		//�������t���猟��
		for (int i = g_dcount - 1; i >= 0; i--)
		{
			//�����f�[�^�ƈ�v����f�[�^����
			if (memcmp(&data[c],g_dic[i].data,g_dic[i].len)==0)
			{
				//LZ78�o�^
				g_lz78[g_lzcount].index = i;

				//�Q�Ɣԍ�
				g_lz78[g_lzcount].data = data[c + g_dic[i].len];

				//�P�f�[�^�ǉ�
				g_lzcount++;

				//�����o�^
				//�f�[�^�擪
				g_dic[g_dcount].data = &data[c];
				//�f�[�^��
				g_dic[g_dcount].len = g_dic[i].len + 1;
				g_dcount++;


				//�����f�[�^�ʒu��i�߂�
				c += g_dic[i].len + 1;

				break;
			}
		}
	}


	//printf("�f�[�^\n\t\"%s\"\n\n����\n", str);//�����f�[�^�̕\��
	//for (int i = 0; i < g_dcount; i++)
	//{
	//	printf("\t%d - %*.*s\n", i, g_dic[i].len, g_dic[i].len, g_dic[i].data);
	//}

	//printf("\nLZ78����\n");					//�����f�[�^�̕\��
	//for (int i = 0; i < g_lzcount; i++)
	//{
	//	printf("\t%d - %c\n", g_lz78[i].index, g_lz78[i].data);
	//}

	// ���k�f�[�^���t�@�C����������
	file = fopen(COMPRESSION_FILE_NAME, "wb");
	fwrite(g_lz78, sizeof(LZ78), g_lzcount, file);
	fclose(file);




	// ����
	// ����f�[�^�������o�^

	
	if (1) {

		// ���k�f�[�^���t�@�C���ǂݍ���

		FILE* file;
		file = fopen(COMPRESSION_FILE_NAME, "rb");

		unsigned int len;

		fseek(file, 0, SEEK_END);
		len = ftell(file);
		fread(g_lz78, len, 1, file);
		fclose(file);

		g_lzcount = len / sizeof(LZ78);

		// ��f�[�^�������o�^
		g_dic[0].data = (unsigned char*)"";
		g_dic[0].len = 0;
		g_dcount = 1;

		unsigned char* orgData;
		orgData = new unsigned char[len * 5];
		c = 0;

		for (int i = 0; i < g_lzcount; i++)
		{
			int index = g_lz78[i].index;

			// �����o�^
			g_dic[g_dcount].data = &orgData[c];
			g_dic[g_dcount].len = g_dic[index].len + 1;
			g_dcount++;

			memcpy(&orgData[c], g_dic[index].data, g_dic[index].len);
			c += g_dic[index].len;

			orgData[c] = g_lz78[i].data;
			c++;
		}

		orgData[c] = 0;

		file = fopen(DECOMPRESSION_FILE_NAME, "wb");
		fwrite(orgData, c, 1, file);
		fclose(file);

		delete[] orgData;
	}


	printf("\nEnter�L�[�ŏI�����܂�\n");
	rewind(stdin);
	getchar();


	return 0;
}