/**
 * @file page.c Paging functions
 *
 * gaim
 *
 * Copyright (C) 2003-2004 Christian Hammond <chipx86@gnupdate.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include "msn.h"
#include "page.h"

#define GET_NEXT(tmp) \
	while (*(tmp) && *(tmp) != ' ' && *(tmp) != '\r') \
		(tmp)++; \
	if (*(tmp) != '\0') *(tmp)++ = '\0'; \
	if (*(tmp) == '\n') *(tmp)++; \
	while (*(tmp) && *(tmp) == ' ') \
		(tmp)++

#define GET_NEXT_LINE(tmp) \
	while (*(tmp) && *(tmp) != '\r') \
		(tmp)++; \
	if (*(tmp) != '\0') *(tmp)++ = '\0'; \
	if (*(tmp) == '\n') *(tmp)++

/*
 * <TEXT xml:space="preserve" enc="utf-8"> == 39
 * </TEXT>                                 ==  7
 *                                           ----
 *                                            46
 */
#define MSN_PAGE_BASE_SIZE 46

MsnPage *
msn_page_new(void)
{
	MsnPage *page;

	page = g_new0(MsnPage, 1);

	page->size = MSN_PAGE_BASE_SIZE;

	return page;
}

void
msn_page_destroy(MsnPage *page)
{
	g_return_if_fail(page != NULL);

	if (page->body != NULL)
		g_free(page->body);

	if (page->from_location != NULL)
		g_free(page->from_location);

	if (page->from_phone != NULL)
		g_free(page->from_phone);

	g_free(page);
}

char *
msn_page_gen_payload(const MsnPage *page, size_t *ret_size)
{
	char *str;

	g_return_val_if_fail(page != NULL, NULL);

	str =
		g_strdup_printf("<TEXT xml:space=\"preserve\" enc=\"utf-8\">%s</TEXT>",
						msn_page_get_body(page));

	if (page->size != strlen(str))
	{
		gaim_debug(GAIM_DEBUG_ERROR, "msn",
				   "Outgoing page size (%d) and string length (%d) "
				   "do not match!\n", page->size, strlen(str));
	}

	if (ret_size != NULL)
		*ret_size = page->size - 1;

	return str;
}

void
msn_page_set_body(MsnPage *page, const char *body)
{
	g_return_if_fail(page != NULL);
	g_return_if_fail(body != NULL);

	if (page->body != NULL)
	{
		page->size -= strlen(page->body);
		g_free(page->body);
	}

	page->body = g_strdup(body);

	page->size += strlen(body);
}

const char *
msn_page_get_body(const MsnPage *page)
{
	g_return_val_if_fail(page != NULL, NULL);

	return page->body;
}

