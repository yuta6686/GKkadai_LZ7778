// LZ78符号化とハフマン
#include <stdio.h>			// printf で必要
#include <string.h>			// memcmp, strlen で必要

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
		// 各バイト値の発生回数を数える
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

		// ハフマン木を作成
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

		// 深さ優先探索でバイト値→符号情報を作成
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

// とりあえず固定長配列で (実用を考えると可変長の必要あり)
#define MAX_DIC		100000	// 辞書最大数
#define MAX_LZ78	100000	// 出力データ最大数

class LZ78Class {
public:

	struct DIC {				// 辞書 (参照番号は添字+1とする)
		unsigned int	len;	// データ長
		unsigned char* data;	// データ先頭 (実体は元のデータ)
	} g_dic[MAX_DIC];
	int	g_dcount = 0;			// 辞書登録数

#pragma pack(1) // パディング（余分データの除去をする)
	struct LZ78 {				// LZ78コード
		unsigned int	index;	// 参照番号
		unsigned char	data;	// データ
	} g_lz78[MAX_LZ78];
#pragma pack()
	int g_lzcount = 0;			// 出力コード数

public:
	void LZ78Compress(const char* comp_file, const char* decomp_file);

	void LZ78Decompress(const char* comp_file, const char* decomp_file);
};

// ファイル名
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
			const string yuuki_str = "【yuukitan.bmp】";

			// LZ78
			cout << yuuki_str  << "Start LZ78 Compress" << endl;
			lz_yuuki->LZ78Compress((YUUKI_FILE_NAME + BMP_EXTENSION).c_str(), (YUUKI_FILE_NAME + LZ_EXTENSION).c_str());

			// ハフマン
			cout << yuuki_str << "Start Huffman Compress" << endl;
			huff_yuuki->HuffmanCompress((YUUKI_FILE_NAME + LZ_EXTENSION).c_str(), (YUUKI_FILE_NAME + LZHUF_EXTENSION).c_str());

			// ハフマン複号
			cout << yuuki_str << "Start Huffman Decompress" << endl;
			huff_yuuki->HuffmanDecomp((YUUKI_FILE_NAME + LZHUF_EXTENSION).c_str(), (YUUKI_FILE_NAME + LZ_EXTENSION).c_str());

			// LZ78 複号
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
			const string hal_str = "【hal.bmp】";

			// LZ78 圧縮
			cout << hal_str << "Start LZ78 Compress" << endl;
			lz_hal->LZ78Compress((HAL_FILE_NAME + BMP_EXTENSION).c_str(), (HAL_FILE_NAME + LZ_EXTENSION).c_str());

			// ハフマン圧縮
			cout << hal_str << "Start Huffman Compress" << endl;
			huff_hal->HuffmanCompress((HAL_FILE_NAME + LZ_EXTENSION).c_str(), (HAL_FILE_NAME + LZHUF_EXTENSION).c_str());

			// ハフマン複号
			cout << hal_str << "Start Huffman Decompress" << endl;
			huff_hal->HuffmanDecomp((HAL_FILE_NAME + LZHUF_EXTENSION).c_str(), (HAL_FILE_NAME + LZ_EXTENSION).c_str());

			// LZ78複号
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

	printf("\nEnterキーで終了します\n");
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


	//空データを辞書登録
	g_dic[0].data = (unsigned char*)"";
	g_dic[0].len = 0;
	g_dcount = 1;

	g_lzcount = 0;

	int c = 0;

	while (c < len)
	{
		//辞書を逆から検索
		for (int i = g_dcount - 1; i >= 0; i--)
		{
			//辞書データと一致するデータ発見
			if (memcmp(&data[c], g_dic[i].data, g_dic[i].len) == 0)
			{
				//LZ78登録
				g_lz78[g_lzcount].index = i;

				//参照番号
				g_lz78[g_lzcount].data = data[c + g_dic[i].len];

				//１データ追加
				g_lzcount++;

				//辞書登録
				//データ先頭
				g_dic[g_dcount].data = &data[c];
				//データ長
				g_dic[g_dcount].len = g_dic[i].len + 1;
				g_dcount++;

				//処理データ位置を進める
				c += g_dic[i].len + 1;

				break;
			}
		}
	}


	// 圧縮データをファイル書き込み
	file = fopen(decomp_file, "wb");
	fwrite(g_lz78, sizeof(LZ78), g_lzcount, file);
	fclose(file);
}

void Huffman::HuffmanCompress(const char* comp_file, const char* decomp_file)
{
	FILE* file;
	file = fopen(comp_file, "rb");
	unsigned int size;					// データサイズ

	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);

	unsigned char* srcData;	// 画像の読み込み用バッファ
	srcData = new unsigned char[size];
	fread(srcData, size, 1, file);	// 配列srcDataへ画像を読み込む
	fclose(file);

	CodeInfo* codeInfo = new CodeInfo[256];
	createValue2Code(codeInfo, srcData, size);

	BitStream* bs = new BitStream;
	compress(bs, srcData, size, codeInfo);

	unsigned char* destData;	//圧縮先のバッファ
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

	// ハフマンコード変換表読み込み
	CodeInfo* codeInfo = new CodeInfo[256];
	fread(codeInfo, sizeof(CodeInfo), 256, file);

	size -= sizeof(CodeInfo) * 256;

	// ハフマン符号化データ読み込み
	unsigned char* srcData;
	srcData = new unsigned char[size];
	fread(srcData, size, 1, file);

	fclose(file);

	// ビットストリーム変換
	BitStream* bs = new BitStream;
	for (unsigned int i = 0; i < size; i++)
	{
		bs->write(srcData[i], 8);
	}

	// 複合
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

	// 複合データー書き込み
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

	// 圧縮データをファイル読み込み

	FILE* file;
	file = fopen(comp_file, "rb");

	unsigned int len;

	fseek(file, 0, SEEK_END);
	len = ftell(file);
	fread(g_lz78, len, 1, file);
	fclose(file);

	g_lzcount = len / sizeof(LZ78);

	// 空データを辞書登録
	g_dic[0].data = (unsigned char*)"";
	g_dic[0].len = 0;
	g_dcount = 1;

	unsigned char* orgData;
	orgData = new unsigned char[len * 5];
	int c = 0;

	for (int i = 0; i < g_lzcount; i++)
	{
		int index = g_lz78[i].index;

		// 辞書登録
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
