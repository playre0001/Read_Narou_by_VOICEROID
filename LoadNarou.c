/*
作成者:playre0001
作成日:20170811
version:1.0
機能概要:Webサイト「小説家になろう」から小説の内容とタイトルを読み込み、ファイルに出力する。
引数概要:Webサイト「小説家になろう」のURL
*/
#include"Header.h"

BOOL readURL(FILE* fp,char* url);
int Checkstr(char a,char* str);
int subCheckstr1(char a,char* str);
int subCheckstr2(char a,char* str);

int LoadNarou(char *url){
	FILE *fp1,*fp2,*fp3;
	char buffer;//文字を読み込む際のバッファ
	int rflag=0;//ルビ検出用タグ
	fpos_t pos1,pos2;//ポジション保存用変数

	//ファイル構造体の読み込み
	if(NULL==(fp1=fopen("keepHTML.txt","w+"))){
		perror("「keepHTML.txt」を開けませんでした。");
		return -1;
	}
	if(NULL==(fp2=fopen("title.txt","w+"))){
		perror("「title.txt」を開けませんでした。");
		return -1;
	}
	if(NULL==(fp3=fopen("novel.txt","w+"))){
		perror("「novel.txt」を開けませんでした。");
		return -1;
	}
	
	printf("webサイトからデータを取得中...\n");
	if (FALSE == readURL(fp1, url)) {//「keepHTML.txt」にwebページのhtmlファイルのデータをを保存
		return -10;
	}
	printf("データの取得完了\n");

	rewind(fp1);//fp1を先頭に戻す

	//タイトルまで読み飛ばす

	printf("タイトルを捜索中...\n");

	while(1){
		buffer=(char)fgetc(fp1);//ファイルから1文字読み出す

		if(1==Checkstr(buffer,"<p class=\"novel_subtitle\">\0")) break;//「<p class="novel_subtitle">」が検出されたらbreak
	}

	printf("タイトルを捜索完了\n");

	//タイトルの読み込み
	printf("タイトルの読み込みを開始...\n");
	
	while(1){
		buffer=(char)fgetc(fp1);

		if(1==Checkstr(buffer,"</p>\0")){//タイトルの記述が終了したら終了
			//書き込んでしまった「</P」を除去
			fseek(fp2,-3,SEEK_CUR);
			fprintf(fp2,"   ");
			
			break;
		}

		fputc((int)buffer,fp2);
	}

	printf("タイトルの読み込みを完了\n");

	//本文まで読み飛ばす
	printf("本文の検索を開始...\n");

	while(1){
		buffer=(char)fgetc(fp1);//ファイルから1文字読み出す

		if(1==Checkstr(buffer,"<div id=\"novel_honbun\" class=\"novel_view\">\0")) break;//「<div id="novel_honbun" class="novel_view">」が検出されたらbreak
	}

	printf("本文を捜索完了\n");

	//本文を読み取る
	printf("本文の読み取りを開始...\n");

	while(1){
		buffer=(char)fgetc(fp1);

		if(1==Checkstr(buffer,"</div>\0")){//本文の記述が終了したら終了
			//書き込んでしまった「</div」を除去
			fseek(fp3,-5,SEEK_CUR);
			fprintf(fp3,"     ");
			
			break;
		}

		if(1==subCheckstr1(buffer,"<br />\0")){//改行タグが来たら無視
			//書き込んでしまった「<br /」の分シークを戻す
			fseek(fp3,-5,SEEK_CUR);
			fprintf(fp3,"\n");
			continue;
		}

		if(rflag==0){
			if(1==subCheckstr2(buffer,"<ruby><rb>\0")){//ルビタグが来たら無視
				//書き込んでしまった「<ruby><rb」の分位置を戻す
				fseek(fp3,-9,SEEK_CUR);
				
				rflag=1;

				if(0!=fgetpos(fp3,&pos1)){//現在の位置を取得
					perror("位置記述示が取得できません。");
					return -3;
				}

				continue;
			}
		}
		else if(rflag==1){
			if(1==subCheckstr2(buffer,"</rp><rt>\0")){//ルビタグの処理
				
				if(0!=fgetpos(fp3,&pos2)){//現在の位置を取得
					perror("位置記述示が取得できません。");
					return -3;
				}

				//書き込んだ分元に戻す
				fseek(fp3,-(pos2-pos1),SEEK_CUR);

				rflag=2;
				continue;
			}
		}
		else if(rflag==2){
			if(1==subCheckstr2(buffer,"</rt><rp>\0")){//ルビタグの処理
				//書き込んでしまった「</rt><rp」の分位置を戻す
				fseek(fp3,-8,SEEK_CUR);
				
				if(0!=fgetpos(fp3,&pos1)){//現在の位置を取得
					perror("位置記述示が取得できません。");
					return -3;
				}
				rflag=3;
				continue;
			}
		}
		else if(rflag==3){
			if(1==subCheckstr2(buffer,"</rp></ruby>\0")){//ルビタグの処理
				if(0!=fgetpos(fp3,&pos2)){//現在の位置を取得
					perror("位置記述示が取得できません。");
					return -3;
				}

				//書き込んだ分元に戻す
				fseek(fp3,-(pos2-pos1),SEEK_CUR);
				
				rflag=0;
				continue;
			}
		}
		else rflag=0;

		//ファイルの改行コードは無視
		if(buffer!='\n') fputc((int)buffer,fp3);

	}

	printf("本文の読み取りを完了\n");

	//ファイルを開放
	fclose(fp1);
	fclose(fp2);
	fclose(fp3);

	return 0;

}

//引数のURLの内容をfpに記述する関数
BOOL readURL(FILE* fp,char* url){
	HINTERNET hInternet,hFile;
	char buffer[BUFFERSIZE+10];
	DWORD ReadSize=BUFFERSIZE;
	BOOL bResalt;

	//WININETの初期化
	hInternet=InternetOpen("ReadURL Program",INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,0);

	//URLのオープン
	if (NULL == (hFile = InternetOpenUrl(hInternet, url, NULL, 0, INTERNET_FLAG_RELOAD, 0))) {
		perror("URLが間違っています。");
		return FALSE;
	}

	//URLの内容をファイルに書き出す
	while(1){
		ReadSize=BUFFERSIZE;//初期値の設定

		bResalt=InternetReadFile(hFile,buffer,BUFFERSIZE,&ReadSize);

		if(bResalt && (ReadSize==0)) break;//読み込みが終了したらループから抜ける

		buffer[ReadSize]='\0';//末端コードを追加

		fwrite(buffer,ReadSize,1,fp);//fpに取得したデータを書き出す。
	}

	//ハンドルを開放
	InternetCloseHandle(hFile);
	InternetCloseHandle(hInternet);

	return TRUE;

}

//1文字ずつ入力される文字がstrと一致したときに1を返す関数。
//2種類の分を同時に使用することはできない
//strの最後には必ず「\0」をつけて入力すること
int Checkstr(char a,char* str){
	static int step=0;//現在どこまで一致しているかを示す変数
	static int doflag=0;//現在文字列の確認を行っているかを判定するフラグ
	static int size=0;
	static char string[1000];

	if(!doflag){//初期化
		int i=0;
		size=0;

		while(1){
			string[i]=str[i];

			if(string[i]=='\0') break;
			size++;
			i++;
		}

		doflag=1;
	}

	//通常の処理
	if(string[step]==a) step++;
	else step=0;

	if(step==size){//文字列が一致した際の処理
		step=0;
		doflag=0;
		return 1;
	}

	return 0;
}

//Checkstrが使えないときに使用する関数。機能は同じ
int subCheckstr1(char a,char* str){
	static int step=0;//現在どこまで一致しているかを示す変数
	static int doflag=0;//現在文字列の確認を行っているかを判定するフラグ
	static int size=0;
	static char string[1000];

	if(!doflag){//初期化
		int i=0;
		size=0;

		while(1){
			string[i]=str[i];

			if(string[i]=='\0') break;
			size++;
			i++;
		}

		doflag=1;
	}

	//通常の処理
	if(string[step]==a) step++;
	else step=0;

	if(step==size){//文字列が一致した際の処理
		step=0;
		doflag=0;
		return 1;
	}

	return 0;
}

//subCheckstr1が使えないときに使用する関数。機能は同じ
int subCheckstr2(char a,char* str){
	static int step=0;//現在どこまで一致しているかを示す変数
	static int doflag=0;//現在文字列の確認を行っているかを判定するフラグ
	static int size=0;
	static char string[1000];

	if(!doflag){//初期化
		int i=0;
		size=0;

		while(1){
			string[i]=str[i];

			if(string[i]=='\0') break;
			size++;
			i++;
		}

		doflag=1;
	}

	//通常の処理
	if(string[step]==a) step++;
	else step=0;

	if(step==size){//文字列が一致した際の処理
		step=0;
		doflag=0;
		return 1;
	}

	return 0;
}