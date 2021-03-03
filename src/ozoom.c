/********************************************************************************
 * MIT License
 *
 * Copyright (c) 2021 Rafael Ribeiro de Carvalho
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *******************************************************************************/

/* Version 1.1 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

enum wdays
{
	DOMINGO=0x0, SEGUNDA=0x1, TERCA=0x2, QUARTA=0x4, QUINTA=0x8, SEXTA=0x10, SABADO=0x20
};

const enum wdays dmap[7] = {DOMINGO, SEGUNDA, TERCA, QUARTA, QUINTA, SEXTA, SABADO};

typedef struct s_class
{
	char codigo_disciplina[8];
	char meeting_id[12];
	char meeting_pass[33];
	unsigned char dias;
	int time;

} Class;

int main(int argc, char *argv[])
{
	time_t t;
	struct tm tm;
	int n_classes = 7, i;

	Class classes[7] = {
		{.codigo_disciplina="FOO1234", .meeting_id="91749443861", .meeting_pass="XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX", .dias = (SEGUNDA | QUARTA | SEXTA), .time = 11},
		{.codigo_disciplina="FOO1234", .meeting_id="00000000000", .meeting_pass="XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX", .dias = (SEGUNDA | QUARTA), .time = 15},
		{.codigo_disciplina="FOO1234", .meeting_id="00000000000", .meeting_pass="XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX", .dias = (TERCA | QUINTA), .time = 7},
		{.codigo_disciplina="FOO1234", .meeting_id="00000000000", .meeting_pass="XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX", .dias = (TERCA | QUINTA), .time = 9},
		{.codigo_disciplina="FOO1234", .meeting_id="00000000000", .meeting_pass="XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX", .dias = (TERCA), .time = 13},
		{.codigo_disciplina="FOO1234", .meeting_id="00000000000", .meeting_pass="XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX", .dias = (QUINTA), .time = 13},
		{.codigo_disciplina="FOO1234", .meeting_id="00000000000", .meeting_pass="XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX", .dias = (TERCA | QUINTA), .time = 15},
	};

	t = time(NULL);
	tm = *localtime(&t);

	for (i = 0; i < n_classes; ++i) {
		if (classes[i].dias & dmap[tm.tm_wday] && ((tm.tm_hour >= classes[i].time && tm.tm_hour < classes[i].time+2) || (tm.tm_hour == classes[i].time-1 && tm.tm_min >= 55) )) {
			char link[84], cmd[95];

			snprintf(link, 84, "zoommtg://zoom.us/join?confno=%s&pwd=%s&zc=0", classes[i].meeting_id, classes[i].meeting_pass);
			printf("Aula %s (%02d:00), zoom link: %s\nDeseja abrir o Zoom? [Y/n] ", classes[i].codigo_disciplina, classes[i].time, link);

			if (getchar() == 'n') continue;

			printf("Abrindo Zoom...\n");

#ifdef __linux__
			snprintf(cmd, 95, "xdg-open \"%s\"", link);
#elif _WIN32
			snprintf(cmd, 95, "start \"\" \"%s\"", link);
#else
			fprintf(stderr, "ERRO: Sistema Operacional não reconhecido/suportado!\n");
			exit(EXIT_FAILURE);
#endif


#ifndef DEBUG
			system(cmd);
#else
			printf("$ %s\n", cmd);
#endif

			break;
		}
	}

	return 0;
}
