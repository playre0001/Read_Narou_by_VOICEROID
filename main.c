/*
�쐬��:playre0001
�쐬��:20170811
version:1.0
�@�\�T�v:Web�T�C�g�u�����ƂɂȂ낤�v���珬���̓��e�ƃ^�C�g����ǂݍ��݁A�t�@�C���ɏo�͂���B
�����T�v:�u�����ƂɂȂ낤�v��URL �J�n�y�[�W���@�I���y�[�W��
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
		perror("����������܂���B");
		return -1;
	}

	for (i = atoi(argv[2]); i < atoi(argv[3]); i++) {
		sprintf(buffer, "%s%d/", argv[1], (i + 1));
		if (LoadNarou(buffer) < 0) {
			perror("NoadNarou");
			return -2;
		}

		//�t�@�C�����e�̐��`
		if (NULL == (fp = _wfopen(OPENFILENAME, L"rt,ccs=UTF-8"))) {
			perror("�t�@�C���̓ǂݍ��݂Ɏ��s�B");
			return -3;
		}

		fgetpos(fp, &pos1);

		while (WEOF != (w = fgetwc(fp))) {//�����C��
			fgetpos(fp, &pos2);

			if (w == L'�`') {
				w = L'�[';
				fseek(fp, pos2 - pos1, SEEK_CUR);
				fputwc(w, fp);
			}

			pos1 = pos2;
		}

		fclose(fp);

		//ini�t�@�C������f�[�^��ǂݍ���
		if (NULL == (fp = fopen("set.ini", "r"))) {
			perror("�t�@�C���̓ǂݍ��݂Ɏ��s�B");
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