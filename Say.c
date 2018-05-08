/*
作成者:playre0001
作成日:20170820
version:1.0
機能概要:指定されたファイルの内容をクリップボードに貼り付け、対象のアプリケーションに、「Ctrl+V」を送り、その後「F5」を送る。
引数概要:対象アプリケーションのウィンドウ名
注意事項:文字コードはUTF-8のみ対応
*/
#include"Header.h"

BOOL CALLBACK CheckHWND(HWND hwnd, LPARAM lp) {
	char windowname[256];

	GetWindowText(hwnd, windowname, sizeof(windowname));//現在のウィンドウのウィンドウ名を取得

	if (strcmp(windowname, ((param*)lp)->windowname) == 0) ((param*)lp)->hwnd = hwnd;//ターゲットのウィンドウ名と一致していた場合、ウィンドウハンドルを格納

	return TRUE;

}

int Say(char *targetname) {
	HGLOBAL hmemory;//メモリ領域のハンドル
	wchar_t* pmemory;//メモリブロック先頭のポインタを格納する変数

	FILE* fp;//ファイルポインタ
	long filesize;//ファイルサイズを格納する変数
	wchar_t* filetext;//ファイルテキストを格納する変数

	static param target = {NULL,"NULL"};

	int i = 0;//ループ用カウンタ

	//ファイルの読み込み
	if (NULL == (fp = _wfopen(OPENFILENAME, L"rt,ccs=UTF-8"))) {
		perror("ファイルの読み込みに失敗。");
		return -2;
	}

	//ファイルサイズの確認
	fseek(fp, 0, SEEK_END);//ファイル位置記述示を最後に持ってくる
	filesize = ftell(fp);//ファイル記述示の位置を取得
	fseek(fp, 0, SEEK_SET);//ファイル位置記述示を最初に持ってくる

						   //DWORD型(unsigned long)に直すための処理(long -> unsigned long)
	if (filesize<0) filesize *= -1;

	filesize += 10;//考えるの面倒だから多めに確保

				   //ファイルの内容の読み込み
	if (NULL == (filetext = (wchar_t*)malloc(sizeof(wchar_t)*(size_t)(filesize)))) {
		perror("メモリ領域を確保できませんでした。");
		return -3;
	}

	while (WEOF != (filetext[i] = fgetwc(fp))) i++;
	filetext[i + 1] = NULL;//クリップボードのデータは末端がNULLで終了する。

	fclose(fp);

	//メモリ領域の確保
	if (NULL == (hmemory = GlobalAlloc(GHND, (DWORD)filesize))) {
		perror("メモリ領域を確保できませんでした。");
		free(filetext);
		return -3;
	}

	//メモリをロック
	if (NULL == (pmemory = (wchar_t*)GlobalLock(hmemory))) {
		perror("メモリをロックできませんでした。");
		free(filetext);
		return -4;
	}

	//メモリにファイルの内容をコピー
	if (NULL == wcscpy(pmemory, filetext)) {
		perror("コピーに失敗");
		free(filetext);
		GlobalUnlock(hmemory);
		return -5;
	}

	//メモリを開放
	GlobalUnlock(hmemory);

	//クリップボードにデータを転送
	while (!OpenClipboard(NULL));//クリップボードが操作できるようになるまで待つ

	if (0 == EmptyClipboard()) {//クリップボードの中身を消す
		perror("クリップボードの初期化に失敗");
		free(filetext);
		GlobalUnlock(hmemory);
		CloseClipboard();
		return -6;
	}

	if (NULL == SetClipboardData(CF_UNICODETEXT, hmemory)) {//クリップボードにデータを転送
		perror("クリップボードにデータを格納できませんでした。");
		free(filetext);
		GlobalUnlock(hmemory);
		CloseClipboard();
		return -7;
	}

	CloseClipboard();//クリップボードの操作権を開放

	if (target.hwnd == NULL) {

		//指定されたプログラムのウィンドウハンドルを取得
		strcpy(target.windowname, targetname);

		EnumWindows(CheckHWND, (LPARAM)&target);//現在起動しているすべてのウィンドウに対してCheckHWND関数を実行

		if (target.hwnd == NULL) {
			sprintf(target.windowname, "%s*", target.windowname);
			EnumWindows(CheckHWND, (LPARAM)&target);//現在起動しているすべてのウィンドウに対してCheckHWND関数を実行
		}
	}

	if (target.hwnd != NULL) {
		INPUT in;//SendInput用の構造

		in.type = INPUT_KEYBOARD;//キーボードからの入力
		in.ki.wScan = 0;//その他は全部0
		in.ki.time = 0;//その他は全部0
		in.ki.dwExtraInfo = 0;//その他は全部0

		//指定されたプログラムをフォアグラウンドウィンドウに設定
		SetForegroundWindow(target.hwnd);

		//フォアグランドウィンドウに指定した動作をシミュレーション

		//Ctrl押す
		in.ki.wVk = VK_CONTROL;//仮想キーコードを入力
		in.ki.dwFlags = 0;//キーを押すときは0, 離すときにはKEYEVENTF_KEYUP
		SendInput(1, &in, sizeof(INPUT));

		//A押す
		in.ki.wVk = 0x41;//仮想キーコードを入力
		in.ki.dwFlags = 0;//キーを押すときは0, 離すときにはKEYEVENTF_KEYUP
		SendInput(1, &in, sizeof(INPUT));

		//Ctrl離す
		in.ki.wVk = VK_CONTROL;//仮想キーコードを入力
		in.ki.dwFlags = KEYEVENTF_KEYUP;//キーを押すときは0, 離すときにはKEYEVENTF_KEYUP
		SendInput(1, &in, sizeof(INPUT));

		//A離す
		in.ki.wVk = 0x41;//仮想キーコードを入力
		in.ki.dwFlags = KEYEVENTF_KEYUP;//キーを押すときは0, 離すときにはKEYEVENTF_KEYUP
		SendInput(1, &in, sizeof(INPUT));

		//BackSpace押す
		in.ki.wVk = VK_BACK;//仮想キーコードを入力
		in.ki.dwFlags = 0;//キーを押すときは0, 離すときにはKEYEVENTF_KEYUP
		SendInput(1, &in, sizeof(INPUT));

		//BackSpace離す
		in.ki.wVk = VK_BACK;//仮想キーコードを入力
		in.ki.dwFlags = KEYEVENTF_KEYUP;//キーを押すときは0, 離すときにはKEYEVENTF_KEYUP
		SendInput(1, &in, sizeof(INPUT));

		//Ctrl押す
		in.ki.wVk = VK_CONTROL;//仮想キーコードを入力
		in.ki.dwFlags = 0;//キーを押すときは0, 離すときにはKEYEVENTF_KEYUP
		SendInput(1, &in, sizeof(INPUT));

		//V押す
		in.ki.wVk = 0x56;//仮想キーコードを入力
		in.ki.dwFlags = 0;//キーを押すときは0, 離すときにはKEYEVENTF_KEYUP
		SendInput(1, &in, sizeof(INPUT));

		//Ctrl離す
		in.ki.wVk = VK_CONTROL;//仮想キーコードを入力
		in.ki.dwFlags = KEYEVENTF_KEYUP;//キーを押すときは0, 離すときにはKEYEVENTF_KEYUP
		SendInput(1, &in, sizeof(INPUT));

		//V離す
		in.ki.wVk = 0x56;//仮想キーコードを入力
		in.ki.dwFlags = KEYEVENTF_KEYUP;//キーを押すときは0, 離すときにはKEYEVENTF_KEYUP
		SendInput(1, &in, sizeof(INPUT));

		//F5押す
		in.ki.wVk = VK_F5;//仮想キーコードを入力
		in.ki.dwFlags = 0;//キーを押すときは0, 離すときにはKEYEVENTF_KEYUP
		SendInput(1, &in, sizeof(INPUT));

		//F5離す
		in.ki.wVk = VK_F5;//仮想キーコードを入力
		in.ki.dwFlags = KEYEVENTF_KEYUP;//キーを押すときは0, 離すときにはKEYEVENTF_KEYUP
		SendInput(1, &in, sizeof(INPUT));

	}
	else {
		perror("指定されたアプリケーションが起動していません。");
		return -8;
	}

	return 0;
}
