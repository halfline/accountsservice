/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2012 Matthew Monaco
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

/*
 * AccountsService config file handler
 *
 * This is designed so that cfg_init may return NULL (due to error...) yet the
 * getters should still work.
 *
 */

#include "util.h"

#define CFG_PATH_LIB          PKGLIBDIR "/accountsservice.conf"
#define CFG_PATH_ETC          CONFDIR   "/accountsservice.conf"
#define CFG_DEFAULT_MIN_UID   500

struct _cfg_struct {
        GKeyFile*      file_etc;
        GKeyFile*      file_lib;
        uid_t          min_uid;
};

static void _cfg_parse_min_uid(Config*);

/**
 * Alloc/Free
**/

Config *cfg_init()
{
        Config*   cfg  = NULL;

        if ((cfg = g_malloc(sizeof(Config))) == NULL)
                return NULL;

        cfg->file_etc  = g_key_file_new();
        cfg->file_lib  = g_key_file_new();
        cfg->min_uid   = CFG_DEFAULT_MIN_UID;

        /* load config from /etc */

        if (g_key_file_load_from_file(cfg->file_etc, CFG_PATH_ETC, G_KEY_FILE_NONE, NULL)) {

                g_key_file_set_list_separator(cfg->file_etc, ' ');

        } else {

                g_key_file_free(cfg->file_etc);
                cfg->file_etc = NULL;
        }

        /* load config from /lib */

        if (g_key_file_load_from_file(cfg->file_lib, CFG_PATH_LIB, G_KEY_FILE_NONE, NULL)) {

                g_key_file_set_list_separator(cfg->file_lib, ' ');

        } else {

                g_key_file_free(cfg->file_lib);
                cfg->file_lib = NULL;
        }

        /* parse options */

        _cfg_parse_min_uid(cfg);

        /* with the above code, these aren't needed anymore */

        g_key_file_free(cfg->file_etc);
        g_key_file_free(cfg->file_lib);
        cfg->file_etc = NULL;
        cfg->file_lib = NULL;

        return cfg;
}

void cfg_free(Config *cfg)
{
        g_key_file_free(cfg->file_etc);
        g_key_file_free(cfg->file_lib);
        g_free(cfg);
}

/**
 * Getters
**/

uid_t cfg_get_min_uid(const Config *cfg)
{
        if (cfg)
                return cfg->min_uid;
        else
                return CFG_DEFAULT_MIN_UID;
}

/**
 * Parsers
**/

static void _cfg_parse_min_uid(Config* cfg)
{
        GError* err;
        int i;

        /* etc takes priority over lib */

        if (cfg->file_etc) {
                err = NULL;
                i = g_key_file_get_integer(cfg->file_etc, "Accounts", "MinimumUID", &err);
                if (err == NULL) {
                        cfg->min_uid = (uid_t) ( (i >= 0) ? i : 0 );
                        return;
                }
        }

        if (cfg->file_lib) {
                err = NULL;
                i = g_key_file_get_integer(cfg->file_lib, "Accounts", "MinimumUID", &err);
                if (err == NULL) {
                        cfg->min_uid = (uid_t) ( (i >= 0) ? i : 0 );
                        return;
                }
        }
}

/* vim: set ts=8 sw=8 sts=8 et : */
