// LZ78�������ƃn�t�}��
#include <stdio.h>			// printf �ŕK�v
#include <string.h>			// memcmp, strlen �ŕK�v

#include <queue>
#include <bitset>
#include <iostream>

#include <future>
#include <Windows.h>
#include <time.h>

using namespace std;

struct CodeInfo
{
	int code;
	int codeSize;
};

struct TreeNode
{
	unsigned char value;
	unsigned int occurrence;
	std::vector<TreeNode> children;
};

bool operator>(const TreeNode& node1, const TreeNode& node2)
{
	return node1.occurrence > node2.occurrence;
}

class Huffman {
public:
	Huffman() = default;
	~Huffman() = default;



	void dfs(CodeInfo* value2code, TreeNode node, int code, int codeSize)
	{
		if (node.children.empty())
		{
			value2code[node.value].code = code;
			value2code[node.value].codeSize = codeSize;
		}
		else {
			dfs(value2code, node.children[0], code << 1, codeSize + 1);
			dfs(value2code, node.children[1], (code << 1) + 1, codeSize + 1);
		}
	}

	void createValue2Code(CodeInfo* value2code, const unsigned char* data, unsigned int size)
	{
		// �e�o�C�g�l�̔����񐔂𐔂���
		TreeNode nodes[256];

		for (int i = 0; i < 256; i++)
		{
			nodes[i].value = i;
			nodes[i].occurrence = 0;
		}

		for (int i = 0; i < size; i++)
		{
			unsigned char v = data[i];
			nodes[v].occurrence++;
		}

		// �n�t�}���؂��쐬
		std::priority_queue<TreeNode, std::vector<TreeNode>, std::greater<TreeNode>> queue;
		for (unsigned int i = 0; i < 256; i++)
		{
			queue.push(nodes[i]);
		}

		while (queue.size() > 1)
		{
			TreeNode n1 = queue.top(); queue.pop();
			TreeNode n2 = queue.top(); queue.pop();

			TreeNode node;
			node.occurrence = n1.occurrence + n2.occurrence;
			node.children.push_back(n1);
			node.children.push_back(n2);

			queue.push(node);
		}

		TreeNode root = queue.top();

		// �[���D��T���Ńo�C�g�l�����������쐬
		dfs(value2code, root, 0, 0);
	}

	class BitStream
	{
	public:
		std::bitset<10000000> bs;
		unsigned int len = 0;
		unsigned int pos = 0;

		void write(unsigned int v, unsigned int bits)
		{
			for (unsigned int i = 0; i < bits; i++)
			{
				bs[len++] = ((v >> (bits - i - 1)) & 1);
			}
		}
		unsigned int read(int bits)
		{
			unsigned int v = 0;
			for (int i = 0; i < bits; i++)
			{
				if (bs[pos++])
				{
					v |= 1 << (bits - i - 1);
				}
			}
			return v;
		}
		unsigned int check(unsigned int bits)
		{
			unsigned int v = 0;
			for (unsigned int i = 0; i < bits; i++)
			{
				if (bs[pos + i])
				{
					v |= 1 << (bits - i - 1);
				}
			}
			return v;
		}
		bool empty()
		{
			if (len <= pos)
				return true;
			else
				return false;
		}
	};

	void compress(BitStream* bs, const unsigned char* data, unsigned int size, const CodeInfo* value2code)
	{
		for (unsigned int i = 0; i < size; i++)
		{
			unsigned char v = data[i];
			CodeInfo codeInfo = value2code[v];
			bs->write(codeInfo.code, codeInfo.codeSize);
		}
	}
public:
	void HuffmanCompress(const char* comp_file, const char* decomp_file);
	void HuffmanDecomp(const char* comp_file, const char* decomp_file);
};

// �Ƃ肠�����Œ蒷�z��� (���p���l����Ɖϒ��̕K�v����)
#define MAX_DIC		100000	// �����ő吔
#define MAX_LZ78	100000	// �o�̓f�[�^�ő吔

class LZ78Class {
public:

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

public:
	void LZ78Compress(const char* comp_file, const char* decomp_file);

	void LZ78Decompress(const char* comp_file, const char* decomp_file);
};

// �t�@�C����
const string YUUKI_FILE_NAME = "yuuki";
const string HAL_FILE_NAME = "hal";
const string BMP_EXTENSION = ".bmp";
const string LZ_EXTENSION = ".lz";
const string LZHUF_EXTENSION = ".lzhuh";
const string DECOMP_EXTENSION = "_decomp.bmp";

int main(void)
{
	clock_t start = clock();

	future yuuki = async(launch::async, []() {
		Huffman* huff_yuuki = new Huffman();
		LZ78Class* lz_yuuki = new LZ78Class();

		// Yuukitan
		cout << "yuukitan.bmp" << endl;
		{
			const string yuuki_str = "�yyuukitan.bmp�z";

			// LZ78
			cout << yuuki_str  << "Start LZ78 Compress" << endl;
			lz_yuuki->LZ78Compress((YUUKI_FILE_NAME + BMP_EXTENSION).c_str(), (YUUKI_FILE_NAME + LZ_EXTENSION).c_str());

			// �n�t�}��
			cout << yuuki_str << "Start Huffman Compress" << endl;
			huff_yuuki->HuffmanCompress((YUUKI_FILE_NAME + LZ_EXTENSION).c_str(), (YUUKI_FILE_NAME + LZHUF_EXTENSION).c_str());

			// �n�t�}������
			cout << yuuki_str << "Start Huffman Decompress" << endl;
			huff_yuuki->HuffmanDecomp((YUUKI_FILE_NAME + LZHUF_EXTENSION).c_str(), (YUUKI_FILE_NAME + LZ_EXTENSION).c_str());

			// LZ78 ����
			cout << yuuki_str << "Start LZ78 Decompress" << endl;
			lz_yuuki->LZ78Decompress((YUUKI_FILE_NAME + LZ_EXTENSION).c_str(), (YUUKI_FILE_NAME + DECOMP_EXTENSION).c_str());
	}

		delete huff_yuuki;
		delete lz_yuuki;
		});

	future hal = async(launch::async, []() {

		Huffman* huff_hal = new Huffman();
		LZ78Class* lz_hal = new LZ78Class();

		// Hal
		cout << "hal.bmp" << endl;
		{
			const string hal_str = "�yhal.bmp�z";

			// LZ78 ���k
			cout << hal_str << "Start LZ78 Compress" << endl;
			lz_hal->LZ78Compress((HAL_FILE_NAME + BMP_EXTENSION).c_str(), (HAL_FILE_NAME + LZ_EXTENSION).c_str());

			// �n�t�}�����k
			cout << hal_str << "Start Huffman Compress" << endl;
			huff_hal->HuffmanCompress((HAL_FILE_NAME + LZ_EXTENSION).c_str(), (HAL_FILE_NAME + LZHUF_EXTENSION).c_str());

			// �n�t�}������
			cout << hal_str << "Start Huffman Decompress" << endl;
			huff_hal->HuffmanDecomp((HAL_FILE_NAME + LZHUF_EXTENSION).c_str(), (HAL_FILE_NAME + LZ_EXTENSION).c_str());

			// LZ78����
			cout << hal_str << "Start LZ78 Decompress" << endl;
			lz_hal->LZ78Decompress((HAL_FILE_NAME + LZ_EXTENSION).c_str(), (HAL_FILE_NAME + DECOMP_EXTENSION).c_str());
		}

		delete huff_hal;
		delete lz_hal;
		});

	yuuki.get();
	hal.get();

	clock_t end = clock();

	cout << "duration = " << (double)(end - start) / CLOCKS_PER_SEC << "sec." << endl;

	printf("\nEnter�L�[�ŏI�����܂�\n");
	rewind(stdin);
	getchar();


	return 0;
}


void LZ78Class::LZ78Compress(const char* comp_file, const char* decomp_file)
{
	FILE* file;
	file = fopen(comp_file, "rb");

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
			if (memcmp(&data[c], g_dic[i].data, g_dic[i].len) == 0)
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


	// ���k�f�[�^���t�@�C����������
	file = fopen(decomp_file, "wb");
	fwrite(g_lz78, sizeof(LZ78), g_lzcount, file);
	fclose(file);
}

void Huffman::HuffmanCompress(const char* comp_file, const char* decomp_file)
{
	FILE* file;
	file = fopen(comp_file, "rb");
	unsigned int size;					// �f�[�^�T�C�Y

	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);

	unsigned char* srcData;	// �摜�̓ǂݍ��ݗp�o�b�t�@
	srcData = new unsigned char[size];
	fread(srcData, size, 1, file);	// �z��srcData�։摜��ǂݍ���
	fclose(file);

	CodeInfo* codeInfo = new CodeInfo[256];
	createValue2Code(codeInfo, srcData, size);

	BitStream* bs = new BitStream;
	compress(bs, srcData, size, codeInfo);

	unsigned char* destData;	//���k��̃o�b�t�@
	destData = new unsigned char[size];
	unsigned int current = 0;
	while (!bs->empty())
	{
		destData[current] = bs->read(8);
		current++;
	}

	file = fopen(decomp_file, "wb");
	fwrite(codeInfo, sizeof(CodeInfo), 256, file);
	fwrite(destData, current, 1, file);
	fclose(file);

	delete[] srcData;
	delete[] codeInfo;
	delete bs;
}

void Huffman::HuffmanDecomp(const char* comp_file,const char* decomp_file)
{
	FILE* file;
	file = fopen(comp_file, "rb");
	unsigned int size;

	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);

	// �n�t�}���R�[�h�ϊ��\�ǂݍ���
	CodeInfo* codeInfo = new CodeInfo[256];
	fread(codeInfo, sizeof(CodeInfo), 256, file);

	size -= sizeof(CodeInfo) * 256;

	// �n�t�}���������f�[�^�ǂݍ���
	unsigned char* srcData;
	srcData = new unsigned char[size];
	fread(srcData, size, 1, file);

	fclose(file);

	// �r�b�g�X�g���[���ϊ�
	BitStream* bs = new BitStream;
	for (unsigned int i = 0; i < size; i++)
	{
		bs->write(srcData[i], 8);
	}

	// ����
	unsigned char* destData;
	destData = new unsigned char[size * 8];
	unsigned int current = 0;

	while (!bs->empty())
	{
		for (int i = 0; i < 256; i++)
		{
			CodeInfo code = codeInfo[i];
			unsigned int v = bs->check(code.codeSize);
			if (v == code.code)
			{
				destData[current] = i;
				current++;
				bs->read(code.codeSize);
				break;
			}
		}
	}

	// �����f�[�^�[��������
	file = fopen(decomp_file, "wb");
	fwrite(destData, current, 1, file);
	fclose(file);

	delete[] srcData;
	delete[] destData;
	delete[] codeInfo;
	delete bs;
}

void LZ78Class::LZ78Decompress(const char* comp_file, const char* decomp_file)
{

	// ���k�f�[�^���t�@�C���ǂݍ���

	FILE* file;
	file = fopen(comp_file, "rb");

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
	int c = 0;

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

	file = fopen(decomp_file, "wb");
	fwrite(orgData, c, 1, file);
	fclose(file);

	delete[] orgData;
}
