/*
Simple RGB to BMP converter by David Cernak
Based on BMP specification found here: http://www.dragonwins.com/domains/getteched/bmp/bmpfileformat.htm
returns 0 if successful
*/

int raw2bmp(void *raw, char *file,unsigned x,unsigned y){
	FILE *f = fopen(file, "wb");
	if (!f)return 1;
	unsigned zero = 0;
	short one = 1, bpp = 24,pad=(x%4);
	unsigned header2 = 40, header = 54, total = 54 + (x*y * 3) + (y * pad);

	fwrite("BM", 1, 2, f);//BM
	fwrite(&total, 4, 1, f);//total size of file
	fwrite(&zero, 4, 1, f);//unused 4b
	fwrite(&header, 4, 1, f);//pixel ofset

	fwrite(&header2, 4, 1, f);//40b header
	fwrite(&x, 4, 1, f);//width
	fwrite(&y, 4, 1, f);//height
	fwrite(&one, 2, 1, f);//must be 1
	fwrite(&bpp, 2, 1, f);//bits per pixel 24
	fwrite(&zero, 4, 1, f);//compression = 0
	fwrite(&zero, 4, 1, f);//size=0
	fwrite(&zero, 4, 1, f);//preferred x = 0
	fwrite(&zero, 4, 1, f);//preferred y = 0
	fwrite(&zero, 4, 1, f);//clrused
	fwrite(&zero, 4, 1, f);//significant colors

	int i,j;
	unsigned char *bufer = (unsigned char*)raw;
	for (i = y-1; i >= 0; i--){
		for (j = 0; j < x; j++){
			unsigned char *line = bufer + (x*i * 3) + j*3;
			fwrite(line+2, 1, 1, f);
			fwrite(line+1, 1, 1, f);
			fwrite(line, 1, 1, f);
		}
		for (j = 0; j < pad; j++)fwrite(&zero, 1, 1, f);
	}
	fclose(f);
	return 0;
}