// LZ78符号化
#include <stdio.h>			// printf で必要
#include <string.h>			// memcmp, strlen で必要

// とりあえず固定長配列で (実用を考えると可変長の必要あり)
#define MAX_DIC		100000	// 辞書最大数
#define MAX_LZ78	100000	// 出力データ最大数

struct DIC {				// 辞書 (参照番号は添字+1とする)
	unsigned int	len;	// データ長
	unsigned char* data;	// データ先頭 (実体は元のデータ)
} g_dic[MAX_DIC];
int	g_dcount = 0;			// 辞書登録数

#pragma pack(1)
struct LZ78 {				// LZ78コード
	unsigned int	index;	// 参照番号
	unsigned char	data;	// データ
} g_lz78[MAX_LZ78];
#pragma pack()
int g_lzcount = 0;			// 出力コード数



int main(void)
{
	char str[] = "ABCBCBCDEBCDABCD";	//テスト用文字列
	int len = strlen(str);				//文字列の長さ
	unsigned char* data = (unsigned char*)str;//文字列を単なる1バイトデータとして扱う


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
			if (memcmp(&data[c],g_dic[i].data,g_dic[i].len)==0)
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


	printf("データ\n\t\"%s\"\n\n辞書\n", str);//辞書データの表示
	for (int i = 0; i < g_dcount; i++)
	{
		printf("\t%d - %*.*s\n", i, g_dic[i].len, g_dic[i].len, g_dic[i].data);
	}

	printf("\nLZ78符号\n");					//符号データの表示
	for (int i = 0; i < g_lzcount; i++)
	{
		printf("\t%d - %c\n", g_lz78[i].index, g_lz78[i].data);
	}

	// 複号
	// からデータを辞書登録

	g_dic[0].data = (unsigned char*)"";
	g_dic[0].len = 0;
	g_dcount = 1;
	
	unsigned char orgData[256];
	c = 0;

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






	printf("\nEnterキーで終了します\n");
	rewind(stdin);
	getchar();


	return 0;
}