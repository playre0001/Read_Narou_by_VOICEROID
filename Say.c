/*
�쐬��:playre0001
�쐬��:20170820
version:1.0
�@�\�T�v:�w�肳�ꂽ�t�@�C���̓��e���N���b�v�{�[�h�ɓ\��t���A�Ώۂ̃A�v���P�[�V�����ɁA�uCtrl+V�v�𑗂�A���̌�uF5�v�𑗂�B
�����T�v:�ΏۃA�v���P�[�V�����̃E�B���h�E��
���ӎ���:�����R�[�h��UTF-8�̂ݑΉ�
*/
#include"Header.h"

BOOL CALLBACK CheckHWND(HWND hwnd, LPARAM lp) {
	char windowname[256];

	GetWindowText(hwnd, windowname, sizeof(windowname));//���݂̃E�B���h�E�̃E�B���h�E�����擾

	if (strcmp(windowname, ((param*)lp)->windowname) == 0) ((param*)lp)->hwnd = hwnd;//�^�[�Q�b�g�̃E�B���h�E���ƈ�v���Ă����ꍇ�A�E�B���h�E�n���h�����i�[

	return TRUE;

}

int Say(char *targetname) {
	HGLOBAL hmemory;//�������̈�̃n���h��
	wchar_t* pmemory;//�������u���b�N�擪�̃|�C���^���i�[����ϐ�

	FILE* fp;//�t�@�C���|�C���^
	long filesize;//�t�@�C���T�C�Y���i�[����ϐ�
	wchar_t* filetext;//�t�@�C���e�L�X�g���i�[����ϐ�

	static param target = {NULL,"NULL"};

	int i = 0;//���[�v�p�J�E���^

	//�t�@�C���̓ǂݍ���
	if (NULL == (fp = _wfopen(OPENFILENAME, L"rt,ccs=UTF-8"))) {
		perror("�t�@�C���̓ǂݍ��݂Ɏ��s�B");
		return -2;
	}

	//�t�@�C���T�C�Y�̊m�F
	fseek(fp, 0, SEEK_END);//�t�@�C���ʒu�L�q�����Ō�Ɏ����Ă���
	filesize = ftell(fp);//�t�@�C���L�q���̈ʒu���擾
	fseek(fp, 0, SEEK_SET);//�t�@�C���ʒu�L�q�����ŏ��Ɏ����Ă���

						   //DWORD�^(unsigned long)�ɒ������߂̏���(long -> unsigned long)
	if (filesize<0) filesize *= -1;

	filesize += 10;//�l����̖ʓ|�����瑽�߂Ɋm��

				   //�t�@�C���̓��e�̓ǂݍ���
	if (NULL == (filetext = (wchar_t*)malloc(sizeof(wchar_t)*(size_t)(filesize)))) {
		perror("�������̈���m�ۂł��܂���ł����B");
		return -3;
	}

	while (WEOF != (filetext[i] = fgetwc(fp))) i++;
	filetext[i + 1] = NULL;//�N���b�v�{�[�h�̃f�[�^�͖��[��NULL�ŏI������B

	fclose(fp);

	//�������̈�̊m��
	if (NULL == (hmemory = GlobalAlloc(GHND, (DWORD)filesize))) {
		perror("�������̈���m�ۂł��܂���ł����B");
		free(filetext);
		return -3;
	}

	//�����������b�N
	if (NULL == (pmemory = (wchar_t*)GlobalLock(hmemory))) {
		perror("�����������b�N�ł��܂���ł����B");
		free(filetext);
		return -4;
	}

	//�������Ƀt�@�C���̓��e���R�s�[
	if (NULL == wcscpy(pmemory, filetext)) {
		perror("�R�s�[�Ɏ��s");
		free(filetext);
		GlobalUnlock(hmemory);
		return -5;
	}

	//���������J��
	GlobalUnlock(hmemory);

	//�N���b�v�{�[�h�Ƀf�[�^��]��
	while (!OpenClipboard(NULL));//�N���b�v�{�[�h������ł���悤�ɂȂ�܂ő҂�

	if (0 == EmptyClipboard()) {//�N���b�v�{�[�h�̒��g������
		perror("�N���b�v�{�[�h�̏������Ɏ��s");
		free(filetext);
		GlobalUnlock(hmemory);
		CloseClipboard();
		return -6;
	}

	if (NULL == SetClipboardData(CF_UNICODETEXT, hmemory)) {//�N���b�v�{�[�h�Ƀf�[�^��]��
		perror("�N���b�v�{�[�h�Ƀf�[�^���i�[�ł��܂���ł����B");
		free(filetext);
		GlobalUnlock(hmemory);
		CloseClipboard();
		return -7;
	}

	CloseClipboard();//�N���b�v�{�[�h�̑��쌠���J��

	if (target.hwnd == NULL) {

		//�w�肳�ꂽ�v���O�����̃E�B���h�E�n���h�����擾
		strcpy(target.windowname, targetname);

		EnumWindows(CheckHWND, (LPARAM)&target);//���݋N�����Ă��邷�ׂẴE�B���h�E�ɑ΂���CheckHWND�֐������s

		if (target.hwnd == NULL) {
			sprintf(target.windowname, "%s*", target.windowname);
			EnumWindows(CheckHWND, (LPARAM)&target);//���݋N�����Ă��邷�ׂẴE�B���h�E�ɑ΂���CheckHWND�֐������s
		}
	}

	if (target.hwnd != NULL) {
		INPUT in;//SendInput�p�̍\��

		in.type = INPUT_KEYBOARD;//�L�[�{�[�h����̓���
		in.ki.wScan = 0;//���̑��͑S��0
		in.ki.time = 0;//���̑��͑S��0
		in.ki.dwExtraInfo = 0;//���̑��͑S��0

		//�w�肳�ꂽ�v���O�������t�H�A�O���E���h�E�B���h�E�ɐݒ�
		SetForegroundWindow(target.hwnd);

		//�t�H�A�O�����h�E�B���h�E�Ɏw�肵��������V�~�����[�V����

		//Ctrl����
		in.ki.wVk = VK_CONTROL;//���z�L�[�R�[�h�����
		in.ki.dwFlags = 0;//�L�[�������Ƃ���0, �����Ƃ��ɂ�KEYEVENTF_KEYUP
		SendInput(1, &in, sizeof(INPUT));

		//A����
		in.ki.wVk = 0x41;//���z�L�[�R�[�h�����
		in.ki.dwFlags = 0;//�L�[�������Ƃ���0, �����Ƃ��ɂ�KEYEVENTF_KEYUP
		SendInput(1, &in, sizeof(INPUT));

		//Ctrl����
		in.ki.wVk = VK_CONTROL;//���z�L�[�R�[�h�����
		in.ki.dwFlags = KEYEVENTF_KEYUP;//�L�[�������Ƃ���0, �����Ƃ��ɂ�KEYEVENTF_KEYUP
		SendInput(1, &in, sizeof(INPUT));

		//A����
		in.ki.wVk = 0x41;//���z�L�[�R�[�h�����
		in.ki.dwFlags = KEYEVENTF_KEYUP;//�L�[�������Ƃ���0, �����Ƃ��ɂ�KEYEVENTF_KEYUP
		SendInput(1, &in, sizeof(INPUT));

		//BackSpace����
		in.ki.wVk = VK_BACK;//���z�L�[�R�[�h�����
		in.ki.dwFlags = 0;//�L�[�������Ƃ���0, �����Ƃ��ɂ�KEYEVENTF_KEYUP
		SendInput(1, &in, sizeof(INPUT));

		//BackSpace����
		in.ki.wVk = VK_BACK;//���z�L�[�R�[�h�����
		in.ki.dwFlags = KEYEVENTF_KEYUP;//�L�[�������Ƃ���0, �����Ƃ��ɂ�KEYEVENTF_KEYUP
		SendInput(1, &in, sizeof(INPUT));

		//Ctrl����
		in.ki.wVk = VK_CONTROL;//���z�L�[�R�[�h�����
		in.ki.dwFlags = 0;//�L�[�������Ƃ���0, �����Ƃ��ɂ�KEYEVENTF_KEYUP
		SendInput(1, &in, sizeof(INPUT));

		//V����
		in.ki.wVk = 0x56;//���z�L�[�R�[�h�����
		in.ki.dwFlags = 0;//�L�[�������Ƃ���0, �����Ƃ��ɂ�KEYEVENTF_KEYUP
		SendInput(1, &in, sizeof(INPUT));

		//Ctrl����
		in.ki.wVk = VK_CONTROL;//���z�L�[�R�[�h�����
		in.ki.dwFlags = KEYEVENTF_KEYUP;//�L�[�������Ƃ���0, �����Ƃ��ɂ�KEYEVENTF_KEYUP
		SendInput(1, &in, sizeof(INPUT));

		//V����
		in.ki.wVk = 0x56;//���z�L�[�R�[�h�����
		in.ki.dwFlags = KEYEVENTF_KEYUP;//�L�[�������Ƃ���0, �����Ƃ��ɂ�KEYEVENTF_KEYUP
		SendInput(1, &in, sizeof(INPUT));

		//F5����
		in.ki.wVk = VK_F5;//���z�L�[�R�[�h�����
		in.ki.dwFlags = 0;//�L�[�������Ƃ���0, �����Ƃ��ɂ�KEYEVENTF_KEYUP
		SendInput(1, &in, sizeof(INPUT));

		//F5����
		in.ki.wVk = VK_F5;//���z�L�[�R�[�h�����
		in.ki.dwFlags = KEYEVENTF_KEYUP;//�L�[�������Ƃ���0, �����Ƃ��ɂ�KEYEVENTF_KEYUP
		SendInput(1, &in, sizeof(INPUT));

	}
	else {
		perror("�w�肳�ꂽ�A�v���P�[�V�������N�����Ă��܂���B");
		return -8;
	}

	return 0;
}
