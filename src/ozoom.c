/********************************************************************************
 * MIT License
 *
 * Copyright (c) 2021 Rafael Ribeiro de Carvalho
 * Copyright (c) 1994–2021 Lua.org, PUC-Rio.
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

/* Version 3.0 */

/*
 * BASIC INSTRUCTIONS:
 *
 * This program may be compiled with or without lua support. With lua support
 * you may set a separate file with the class information that does not need
 * to be recompiled when modified. Without lua support you must set the classes
 * array and n_classes variable to its size in main().
 *
 * Compilation with lua support:
 * gcc -llua -O2 -o openzoom ozoom.c
 *
 * Compilation without lua support:
 * gcc -O2 -o openzoom ozoom.c -D _NO_LUA
 *
 * Example lua file (zoom_classes.lua):
 *
 * CLASSES = {
 * 		{discipline_code="FOO1234", meeting_id="00000000000", meeting_pass="xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", days = (SEGUNDA | QUARTA), time = 7},
 * }
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef _NO_LUA
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#define FILE_NAME "zoom_classes.lua"
#define TAB_NAME "CLASSES"
#endif

#define ASSERT(cond, err) if(cond) die(err);

enum wdays
{
	DOMINGO=0x1, SEGUNDA=0x2, TERCA=0x4, QUARTA=0x8, QUINTA=0x10, SEXTA=0x20, SABADO=0x40
};

const enum wdays dmap[7] = {DOMINGO, SEGUNDA, TERCA, QUARTA, QUINTA, SEXTA, SABADO};

typedef struct s_class
{
	char discipline_code[8];
	char meeting_id[12];
	char meeting_pass[33];
	unsigned char days;
	int time;
} Class;

#ifndef _NO_LUA
void lua_create_global_int(lua_State *L, const char *name, int value);
void lua_set_wday_constants(lua_State *L);
int lua_get_table_string(lua_State *L, const char *field_name, char *dest, size_t len);
int lua_get_table_int(lua_State *L, const char *field_name, int *dest);
int lua_get_classes(lua_State *L, Class **classes);
#endif

void die (const char *msg);

int main(void)
{
	time_t t;
	struct tm tm;
	int n_classes, i;
	int p_classesn = 0;
	Class *p_classes[2];
	Class *s_class = NULL;

#ifdef _NO_LUA
	/* EXEMPLO */
	Class classes[1] = {
		{.discipline_code="FOO1234", .meeting_id="00000000000", .meeting_pass="xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", .days = (SEGUNDA | QUARTA | SEXTA), .time = 7},
	};
	n_classes = 1;
#else
	Class *classes;
	lua_State *L = luaL_newstate();
	ASSERT(!L, "luaL_newstate")

	lua_set_wday_constants(L);

	n_classes = lua_get_classes(L, &classes);
#endif

	t = time(NULL);
	tm = *localtime(&t);

	for (i = n_classes-1; i >= 0; --i) {
		if (classes[i].days & dmap[tm.tm_wday]
			&& ((tm.tm_hour >= classes[i].time && tm.tm_hour<classes[i].time+2)
				|| (tm.tm_hour == classes[i].time-1 && tm.tm_min >= 45) ))
		{
			p_classes[p_classesn++] = &classes[i];
			if (p_classesn == 2) break;
		}
	}

#ifndef _NO_LUA
	lua_close(L);
#endif

	switch (p_classesn) {
		case 0:
			fprintf(stderr, "ERRO: nenhuma aula encontrada\n");
			exit(EXIT_FAILURE);
			break;
		case 1:
			s_class = p_classes[0];
			break;
		default:
		{
			char c;
			printf("1. %s (%02d:00)\n2. %s (%02d:00)\n3. CANCELAR\n\n> ",
					p_classes[0]->discipline_code, p_classes[0]->time,
					p_classes[1]->discipline_code, p_classes[1]->time);

			c = getchar();
			switch (c) {
				default:
				case '1':
					s_class = p_classes[0];
					break;
				case '2':
					s_class = p_classes[1];
					break;
				case '3':
					s_class = NULL;
			}
		}
	}


	if (s_class) {
		char link[84], cmd[95];

		snprintf(link, 84, "zoommtg://zoom.us/join?confno=%s&pwd=%s&zc=0", s_class->meeting_id, s_class->meeting_pass);
		printf("Aula %s (%02d:00), zoom link: %s\nDeseja abrir o Zoom? [Y/n] ", s_class->discipline_code, s_class->time, link);

		if (getchar() != 'n') {

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
		}

	} else {
		printf("Nenhuma aula selecionada.\n");
	}

#ifndef _NO_LUA
	free(classes);
#endif

	return 0;
}

#ifndef _NO_LUA
void lua_create_global_int(lua_State *L, const char *name, int value)
{
	lua_pushnumber(L, value);
	lua_setglobal(L, name);
}

void lua_set_wday_constants(lua_State *L)
{
	lua_create_global_int(L, "DOMINGO", dmap[0]);
	lua_create_global_int(L, "SEGUNDA", dmap[1]);
	lua_create_global_int(L, "TERCA", dmap[2]);
	lua_create_global_int(L, "QUARTA", dmap[3]);
	lua_create_global_int(L, "QUINTA", dmap[4]);
	lua_create_global_int(L, "SEXTA", dmap[5]);
	lua_create_global_int(L, "SABADO", dmap[6]);
}

int lua_get_table_string(lua_State *L, const char *field_name, char *dest, size_t len)
{
	const char *ptr;

	lua_pushstring(L, field_name);
	lua_gettable(L, -2);
	ASSERT(!lua_isstring(L, -1), "LUA - missing table field")
	ptr = lua_tostring(L, -1);
	memcpy(dest, ptr, len);
	lua_pop(L, 1);

	return 0;
}

int lua_get_table_int(lua_State *L, const char *field_name, int *dest)
{
	lua_pushstring(L, field_name);
	lua_gettable(L, -2);
	ASSERT(!lua_isnumber(L, -1), "LUA - missing table field")
	*dest = lua_tonumber(L, -1);
	lua_pop(L, 1);

	return 0;
}

int lua_get_classes(lua_State *L, Class **classes)
{
	int classes_num, i;

	ASSERT(luaL_dofile(L, FILE_NAME) != LUA_OK, lua_tostring(L, -1))

	lua_getglobal(L, TAB_NAME);
	ASSERT(!lua_istable(L, -1), "LUA - expected " TAB_NAME "table in lua file")

	classes_num = lua_rawlen(L, -1);

	*classes = (Class *)malloc(sizeof(Class) * classes_num);
	ASSERT(!classes, "malloc - unable to allocate memory")

	for (i = 1; i <= classes_num; ++i) {
		lua_pushnumber(L, i);
		lua_gettable(L, -2);

		lua_get_table_string(L, "discipline_code", (*classes)[i-1].discipline_code, 8);
		lua_get_table_string(L, "meeting_id", (*classes)[i-1].meeting_id, 12);
		lua_get_table_string(L, "meeting_pass", (*classes)[i-1].meeting_pass, 33);

		lua_get_table_int(L, "days", (int *)&(*classes)[i-1].days);
		lua_get_table_int(L, "time", &(*classes)[i-1].time);

		lua_pop(L, 1);
	}

	return classes_num;
}
#endif

void die (const char *msg)
{
	fprintf(stderr, "ERROR: %s.\nAborting...\n", msg);
	exit(EXIT_FAILURE);
}
