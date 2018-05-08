/*
作成者:playre0001
作成日:20170811
version:1.0
機能概要:Webサイト「小説家になろう」から小説の内容とタイトルを読み込み、ファイルに出力する。
引数概要:「小説家になろう」のURL 開始ページ数　終了ページ数
*/

#include"Header.h"


int main(int argc, char *argv[]) {
	int i,k=0;
	char buffer[1000];
	char c;
	FILE *fp;
	wchar_t w;
	fpos_t pos1,pos2;
	int flag=0;

	if (argc < 4) {
		perror("引数が足りません。");
		return -1;
	}

	for (i = atoi(argv[2]); i < atoi(argv[3]); i++) {
		sprintf(buffer, "%s%d/", argv[1], (i + 1));
		if (LoadNarou(buffer) < 0) {
			perror("NoadNarou");
			return -2;
		}

		//ファイル内容の整形
		if (NULL == (fp = _wfopen(OPENFILENAME, L"rt,ccs=UTF-8"))) {
			perror("ファイルの読み込みに失敗。");
			return -3;
		}

		fgetpos(fp, &pos1);

		while (WEOF != (w = fgetwc(fp))) {//文字修正
			fgetpos(fp, &pos2);

			if (w == L'～') {
				w = L'ー';
				fseek(fp, pos2 - pos1, SEEK_CUR);
				fputwc(w, fp);
			}

			pos1 = pos2;
		}

		fclose(fp);

		//iniファイルからデータを読み込み
		if (NULL == (fp = fopen("set.ini", "r"))) {
			perror("ファイルの読み込みに失敗。");
			return -3;
		}

		while (EOF != (c = fgetc(fp))) {
			if (c == ';') {
				if (EOF == (c = fgetc(fp))) if (c == '1') flag = 1;
				break;
			}
			buffer[k] = c;
			
			k++;
		}

		buffer[k] = '\0';

		fclose(fp);

		if (Say(buffer) < 0) {
			perror("Say"); getchar();
			return -3;
		}
		
		if (flag) Sleep(600000);
		else getchar();

	}
	
}