#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <openssl/aes.h>


int main(int argc, char** argv)
{
	/*
	   char user_key[AES_BLOCK_SIZE] = "abc123";
	   AES_KEY key;

	   char p_msq[16] = "abcdefghijkl";
	   char e_msq[17];

	   AES_set_encrypt_key(user_key, AES_BLOCK_SIZE*8, &key);

	   AES_encrypt(p_msq, e_msq, &key);

	   printf("%s\n",e_msq);

	   char msg[16];

	   AES_set_decrypt_key(user_key, 128, &key);
	   AES_decrypt(e_msq, msg, &key);

	   printf("%s\n",msg);
	 */
	if(5 != argc)
	{
		fprintf(stderr, "参数输入错误\n");
		exit(1);
	}

	FILE* fp_plain = NULL;
	FILE* fp_encrypted = NULL;
	AES_KEY key;
	unsigned char p[16], e[16];
	char user_key[17];
	int res = 0;

	if(!strcmp(argv[4],"-e"))
	{
		fp_plain = fopen(argv[1], "rb");
		if(NULL == fp_plain)
		{
			fprintf(stderr, "open %s fail: %s\n", argv[1], strerror(errno));
			exit(1);
		}
		fp_encrypted = fopen(argv[2], "wb");

		strcpy(user_key, argv[3]);
		AES_set_encrypt_key(user_key, 128, &key);

		while(res = fread(p, 1, 16, fp_plain))
		{
			AES_encrypt(p, e, &key);
			fwrite(e, 1, 16, fp_encrypted);
			if(res < 16) break;
		}
		if(res == 0) res = 16;
		sprintf(p, "%d", res);
		AES_encrypt(p, e, &key);
		fwrite(e, 1, 16, fp_encrypted);

		fclose(fp_plain);
		fclose(fp_encrypted);
	}
	else if(!strcmp(argv[4], "-d"))
	{
		int len = 0;
		long end = 0;
		fp_encrypted = fopen(argv[1], "rb");
		if(NULL == fp_encrypted)
		{
			fprintf(stderr, "open %s fail: %s\n", argv[1], strerror(errno));
			exit(1);
		}
		fp_plain = fopen(argv[2], "wb");

		strcpy(user_key, argv[3]);
		AES_set_decrypt_key(user_key, 128, &key);

		fseek(fp_encrypted, -16, SEEK_END);
		end = ftell(fp_encrypted);
		fread(e, 1, 16, fp_encrypted);
		AES_decrypt(e, p, &key);
		len = atoi(p);
		rewind(fp_encrypted);

		while(1)
		{
			fread(e, 1, 16, fp_encrypted);
			AES_decrypt(e, p, &key);
			if(end == ftell(fp_encrypted))
			{
				fwrite(p, 1, len, fp_plain);
				break;
			}
			fwrite(p, 1, 16, fp_plain);
		}

		fclose(fp_plain);
		fclose(fp_encrypted);
	}

	return 0;
}
