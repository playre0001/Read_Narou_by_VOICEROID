/*
�쐬��:playre0001
�쐬��:20170811
version:1.0
�@�\�T�v:Web�T�C�g�u�����ƂɂȂ낤�v���珬���̓��e�ƃ^�C�g����ǂݍ��݁A�t�@�C���ɏo�͂���B
�����T�v:Web�T�C�g�u�����ƂɂȂ낤�v��URL
*/
#include"Header.h"

BOOL readURL(FILE* fp,char* url);
int Checkstr(char a,char* str);
int subCheckstr1(char a,char* str);
int subCheckstr2(char a,char* str);

int LoadNarou(char *url){
	FILE *fp1,*fp2,*fp3;
	char buffer;//������ǂݍ��ލۂ̃o�b�t�@
	int rflag=0;//���r���o�p�^�O
	fpos_t pos1,pos2;//�|�W�V�����ۑ��p�ϐ�

	//�t�@�C���\���̂̓ǂݍ���
	if(NULL==(fp1=fopen("keepHTML.txt","w+"))){
		perror("�ukeepHTML.txt�v���J���܂���ł����B");
		return -1;
	}
	if(NULL==(fp2=fopen("title.txt","w+"))){
		perror("�utitle.txt�v���J���܂���ł����B");
		return -1;
	}
	if(NULL==(fp3=fopen("novel.txt","w+"))){
		perror("�unovel.txt�v���J���܂���ł����B");
		return -1;
	}
	
	printf("web�T�C�g����f�[�^���擾��...\n");
	if (FALSE == readURL(fp1, url)) {//�ukeepHTML.txt�v��web�y�[�W��html�t�@�C���̃f�[�^����ۑ�
		return -10;
	}
	printf("�f�[�^�̎擾����\n");

	rewind(fp1);//fp1��擪�ɖ߂�

	//�^�C�g���܂œǂݔ�΂�

	printf("�^�C�g����{����...\n");

	while(1){
		buffer=(char)fgetc(fp1);//�t�@�C������1�����ǂݏo��

		if(1==Checkstr(buffer,"<p class=\"novel_subtitle\">\0")) break;//�u<p class="novel_subtitle">�v�����o���ꂽ��break
	}

	printf("�^�C�g����{������\n");

	//�^�C�g���̓ǂݍ���
	printf("�^�C�g���̓ǂݍ��݂��J�n...\n");
	
	while(1){
		buffer=(char)fgetc(fp1);

		if(1==Checkstr(buffer,"</p>\0")){//�^�C�g���̋L�q���I��������I��
			//��������ł��܂����u</P�v������
			fseek(fp2,-3,SEEK_CUR);
			fprintf(fp2,"   ");
			
			break;
		}

		fputc((int)buffer,fp2);
	}

	printf("�^�C�g���̓ǂݍ��݂�����\n");

	//�{���܂œǂݔ�΂�
	printf("�{���̌������J�n...\n");

	while(1){
		buffer=(char)fgetc(fp1);//�t�@�C������1�����ǂݏo��

		if(1==Checkstr(buffer,"<div id=\"novel_honbun\" class=\"novel_view\">\0")) break;//�u<div id="novel_honbun" class="novel_view">�v�����o���ꂽ��break
	}

	printf("�{����{������\n");

	//�{����ǂݎ��
	printf("�{���̓ǂݎ����J�n...\n");

	while(1){
		buffer=(char)fgetc(fp1);

		if(1==Checkstr(buffer,"</div>\0")){//�{���̋L�q���I��������I��
			//��������ł��܂����u</div�v������
			fseek(fp3,-5,SEEK_CUR);
			fprintf(fp3,"     ");
			
			break;
		}

		if(1==subCheckstr1(buffer,"<br />\0")){//���s�^�O�������疳��
			//��������ł��܂����u<br /�v�̕��V�[�N��߂�
			fseek(fp3,-5,SEEK_CUR);
			fprintf(fp3,"\n");
			continue;
		}

		if(rflag==0){
			if(1==subCheckstr2(buffer,"<ruby><rb>\0")){//���r�^�O�������疳��
				//��������ł��܂����u<ruby><rb�v�̕��ʒu��߂�
				fseek(fp3,-9,SEEK_CUR);
				
				rflag=1;

				if(0!=fgetpos(fp3,&pos1)){//���݂̈ʒu���擾
					perror("�ʒu�L�q�����擾�ł��܂���B");
					return -3;
				}

				continue;
			}
		}
		else if(rflag==1){
			if(1==subCheckstr2(buffer,"</rp><rt>\0")){//���r�^�O�̏���
				
				if(0!=fgetpos(fp3,&pos2)){//���݂̈ʒu���擾
					perror("�ʒu�L�q�����擾�ł��܂���B");
					return -3;
				}

				//�������񂾕����ɖ߂�
				fseek(fp3,-(pos2-pos1),SEEK_CUR);

				rflag=2;
				continue;
			}
		}
		else if(rflag==2){
			if(1==subCheckstr2(buffer,"</rt><rp>\0")){//���r�^�O�̏���
				//��������ł��܂����u</rt><rp�v�̕��ʒu��߂�
				fseek(fp3,-8,SEEK_CUR);
				
				if(0!=fgetpos(fp3,&pos1)){//���݂̈ʒu���擾
					perror("�ʒu�L�q�����擾�ł��܂���B");
					return -3;
				}
				rflag=3;
				continue;
			}
		}
		else if(rflag==3){
			if(1==subCheckstr2(buffer,"</rp></ruby>\0")){//���r�^�O�̏���
				if(0!=fgetpos(fp3,&pos2)){//���݂̈ʒu���擾
					perror("�ʒu�L�q�����擾�ł��܂���B");
					return -3;
				}

				//�������񂾕����ɖ߂�
				fseek(fp3,-(pos2-pos1),SEEK_CUR);
				
				rflag=0;
				continue;
			}
		}
		else rflag=0;

		//�t�@�C���̉��s�R�[�h�͖���
		if(buffer!='\n') fputc((int)buffer,fp3);

	}

	printf("�{���̓ǂݎ�������\n");

	//�t�@�C�����J��
	fclose(fp1);
	fclose(fp2);
	fclose(fp3);

	return 0;

}

//������URL�̓��e��fp�ɋL�q����֐�
BOOL readURL(FILE* fp,char* url){
	HINTERNET hInternet,hFile;
	char buffer[BUFFERSIZE+10];
	DWORD ReadSize=BUFFERSIZE;
	BOOL bResalt;

	//WININET�̏�����
	hInternet=InternetOpen("ReadURL Program",INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,0);

	//URL�̃I�[�v��
	if (NULL == (hFile = InternetOpenUrl(hInternet, url, NULL, 0, INTERNET_FLAG_RELOAD, 0))) {
		perror("URL���Ԉ���Ă��܂��B");
		return FALSE;
	}

	//URL�̓��e���t�@�C���ɏ����o��
	while(1){
		ReadSize=BUFFERSIZE;//�����l�̐ݒ�

		bResalt=InternetReadFile(hFile,buffer,BUFFERSIZE,&ReadSize);

		if(bResalt && (ReadSize==0)) break;//�ǂݍ��݂��I�������烋�[�v���甲����

		buffer[ReadSize]='\0';//���[�R�[�h��ǉ�

		fwrite(buffer,ReadSize,1,fp);//fp�Ɏ擾�����f�[�^�������o���B
	}

	//�n���h�����J��
	InternetCloseHandle(hFile);
	InternetCloseHandle(hInternet);

	return TRUE;

}

//1���������͂���镶����str�ƈ�v�����Ƃ���1��Ԃ��֐��B
//2��ނ̕��𓯎��Ɏg�p���邱�Ƃ͂ł��Ȃ�
//str�̍Ō�ɂ͕K���u\0�v�����ē��͂��邱��
int Checkstr(char a,char* str){
	static int step=0;//���݂ǂ��܂ň�v���Ă��邩�������ϐ�
	static int doflag=0;//���ݕ�����̊m�F���s���Ă��邩�𔻒肷��t���O
	static int size=0;
	static char string[1000];

	if(!doflag){//������
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

	//�ʏ�̏���
	if(string[step]==a) step++;
	else step=0;

	if(step==size){//�����񂪈�v�����ۂ̏���
		step=0;
		doflag=0;
		return 1;
	}

	return 0;
}

//Checkstr���g���Ȃ��Ƃ��Ɏg�p����֐��B�@�\�͓���
int subCheckstr1(char a,char* str){
	static int step=0;//���݂ǂ��܂ň�v���Ă��邩�������ϐ�
	static int doflag=0;//���ݕ�����̊m�F���s���Ă��邩�𔻒肷��t���O
	static int size=0;
	static char string[1000];

	if(!doflag){//������
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

	//�ʏ�̏���
	if(string[step]==a) step++;
	else step=0;

	if(step==size){//�����񂪈�v�����ۂ̏���
		step=0;
		doflag=0;
		return 1;
	}

	return 0;
}

//subCheckstr1���g���Ȃ��Ƃ��Ɏg�p����֐��B�@�\�͓���
int subCheckstr2(char a,char* str){
	static int step=0;//���݂ǂ��܂ň�v���Ă��邩�������ϐ�
	static int doflag=0;//���ݕ�����̊m�F���s���Ă��邩�𔻒肷��t���O
	static int size=0;
	static char string[1000];

	if(!doflag){//������
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

	//�ʏ�̏���
	if(string[step]==a) step++;
	else step=0;

	if(step==size){//�����񂪈�v�����ۂ̏���
		step=0;
		doflag=0;
		return 1;
	}

	return 0;
}